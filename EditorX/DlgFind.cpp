#include "StdAfx.h"
#include "resource.h"
#include "DlgFind.h"
#include "EditorXFrame.h"
#include "ScintillaEdit.h"
#include "WinSDKUtils.h"
#include "./directui/UIFrame.h"

DlgFind DlgFind::m_instance;
DlgFind::DlgFind(void)
{
	m_hDlgModeless = NULL;
	m_hFontHint = NULL;
	m_hIcon = NULL;

	::memset(&m_rcHint,0,sizeof(m_rcHint));
}


DlgFind::~DlgFind(void)
{
	if (::IsWindow(m_hDlgModeless)) {
		::DestroyWindow(m_hDlgModeless);
	}
	m_hDlgModeless = NULL;

	if (m_hFontHint) {
		::DeleteObject(m_hFontHint);
	}
	m_hFontHint = NULL;
}

DlgFind& DlgFind::GetInstance()
{
	if (!m_instance.m_hDlgModeless) {
		m_instance.m_hDlgModeless = ::CreateDialogParam(EditorXFrame::GetInstance().GetAppInstance()
			, MAKEINTRESOURCE(IDD_FIND)
			, EditorXFrame::GetInstance().GetHWnd()
			, OnDlgMsg
		    , LPARAM(&m_instance));
	}

	return m_instance;
}

void DlgFind::ShowInModeless(const StringUtils::T_STRING& search_text/*=TEXT("")*/)
{
	::SetWindowText(GetDlgItem(m_hDlgModeless,IDC_CMB_SEARCH),search_text.c_str());
	::ShowWindow(m_hDlgModeless, SW_RESTORE);
	::PostMessage(GetDlgItem(m_hDlgModeless, IDC_CMB_SEARCH), WM_SETFOCUS, 0, 0);

	//对话框居中显示
	RECT rcEditor,rcUIFrame;
	rcUIFrame = lcc_direct_ui::UIFrame::GetInstance().GetRect();
	WinSDKUtils::ClientToScreen(EditorXFrame::GetInstance().GetHWnd(),rcUIFrame);
	::GetWindowRect(EditorXFrame::GetInstance().GetHWnd(),&rcEditor);	
	rcEditor.top = rcUIFrame.bottom;
	int x = ((rcEditor.right-rcEditor.left)-(m_rcDlg.right-m_rcDlg.left))/2+rcEditor.left;
	int y = ((rcEditor.bottom-rcEditor.top)-(m_rcDlg.bottom-m_rcDlg.top))/2+rcEditor.top;

	::SetWindowPos(m_hDlgModeless,HWND_TOP,x,y,0,0,SWP_NOSIZE);
}

INT_PTR CALLBACK DlgFind::OnDlgMsg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	DlgFind *pMe = nullptr;

	HDC hdc(NULL);
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_INITDIALOG:
		pMe = (DlgFind *)lParam;

		//设置对话框图标
		if (!(pMe->m_hIcon)) {
			pMe->m_hIcon = ::LoadIcon(EditorXFrame::GetInstance().GetAppInstance(), MAKEINTRESOURCE(IDI_SEARCH));
		}
		::PostMessage(hDlg,WM_SETICON,ICON_SMALL,LPARAM(pMe->m_hIcon));

		// 获取对话框的位置及大小
		::GetWindowRect(hDlg,&(pMe->m_rcDlg));
		
		//设置信息提示框的位置
		pMe->m_rcHint.left = (LONG)WinSDKUtils::GetAdaptationSizeForDPI(23);
		pMe->m_rcHint.top = (LONG)WinSDKUtils::GetAdaptationSizeForDPI(65);
		pMe->m_rcHint.right = (LONG)WinSDKUtils::GetAdaptationSizeForDPI(200);
		pMe->m_rcHint.bottom = (LONG)WinSDKUtils::GetAdaptationSizeForDPI(100);
		pMe->m_hFontHint = ::CreateFont((int)WinSDKUtils::GetAdaptationSizeForDPI(9)
			, 0, 0, 0, FW_NORMAL,0, 0, 0, 0
			, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH
			, TEXT("黑体"));

		//设置默认向下搜索
		::PostMessage(GetDlgItem(hDlg,IDC_RDO_FINDNEXT),BM_SETCHECK,BST_CHECKED,0);
		//设置默认整字匹配
		::PostMessage(GetDlgItem(hDlg,IDC_CHK_WHOLEWORD),BM_SETCHECK,BST_CHECKED,0);
		return (INT_PTR)TRUE;
	case WM_PAINT:
		hdc = BeginPaint(hDlg, &ps);
		if (m_instance.m_hFontHint) { // 显示提示信息		
			::SetTextColor(hdc,RGB(188,0,0));
			::SetBkMode(hdc,TRANSPARENT);
			HGDIOBJ hOldFont = ::SelectObject(hdc,m_instance.m_hFontHint);
			::DrawText(hdc,m_instance.m_strHint.c_str(),-1,&(m_instance.m_rcHint),DT_LEFT);
			::SelectObject(hdc,hOldFont);
		}
		EndPaint(hDlg, &ps);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		case IDC_BTN_FIND: //搜索按钮单击事件
			if (HIWORD(wParam) != BN_CLICKED) {
				break;
			}
			m_instance.Find();
			return (INT_PTR)TRUE;
		case IDC_BTN_REPLACE:  //替换按钮单击事件
		case IDC_BTN_REPLACEALL: //替换全部按钮单击事件
			if (HIWORD(wParam) != BN_CLICKED) {
				break;
			}
			m_instance.Replace((LOWORD(wParam)) == IDC_BTN_REPLACEALL);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

BOOL DlgFind::Find()
{
	ScintillaEdit *pEditor = EditorXFrame::GetInstance().GetActiveEditor();
	if (!pEditor) {
		lcc_direct_ui::UIFrame::GetInstance().Hint(TEXT("当前无活动的编辑器"));
		return FALSE;
	}
	StringUtils::T_STRING str = GetCmbText(IDC_CMB_SEARCH);
	if (str.length() < 1) {
		return FALSE;
	}

	bool matchcase = (::SendMessage(GetDlgItem(m_hDlgModeless,IDC_CHK_MATCHCASE),BM_GETCHECK,0,0) == BST_CHECKED);
	bool wholeword = (::SendMessage(GetDlgItem(m_hDlgModeless,IDC_CHK_WHOLEWORD),BM_GETCHECK,0,0) == BST_CHECKED);
	bool searchnext = (::SendMessage(GetDlgItem(m_hDlgModeless,IDC_RDO_FINDNEXT),BM_GETCHECK,0,0) == BST_CHECKED);
	BOOL result = pEditor->Find(str.c_str(),matchcase,wholeword,searchnext);

	//显示提示信息
	m_strHint = (result > -1) ? TEXT("") : TEXT("未搜索到匹配项！");
	::InvalidateRect(m_hDlgModeless,&m_rcHint,TRUE);

	return (result > -1);
}

BOOL DlgFind::Replace(bool replace_all/*=false*/)
{
	ScintillaEdit *pEditor = EditorXFrame::GetInstance().GetActiveEditor();
	if (!pEditor) {
		lcc_direct_ui::UIFrame::GetInstance().Hint(TEXT("当前无活动的编辑器"));
		return FALSE;
	}
	StringUtils::T_STRING str = GetCmbText(IDC_CMB_SEARCH);
	if (str.length() < 1) {
		return FALSE;
	}
	StringUtils::T_STRING strReplace = GetCmbText(IDC_CMB_REPLACE);

	bool matchcase = (::SendMessage(GetDlgItem(m_hDlgModeless,IDC_CHK_MATCHCASE),BM_GETCHECK,0,0) == BST_CHECKED);
	bool wholeword = (::SendMessage(GetDlgItem(m_hDlgModeless,IDC_CHK_WHOLEWORD),BM_GETCHECK,0,0) == BST_CHECKED);
	bool searchnext = (::SendMessage(GetDlgItem(m_hDlgModeless,IDC_RDO_FINDNEXT),BM_GETCHECK,0,0) == BST_CHECKED);

	int searchtype = replace_all ? 0 : (searchnext ? 1 : 2);
	int result = pEditor->Replace(str.c_str(),strReplace.c_str(),matchcase,wholeword,searchtype);

	//显示提示信息
	if (result > 0) {
		m_strHint = StringUtils::format(TEXT("已替换%d个匹配项"),result);
	} else {
		m_strHint = TEXT("未搜索到匹配项！");
	}
	::InvalidateRect(m_hDlgModeless,&m_rcHint,TRUE);

	return (result > -1);
}

StringUtils::T_STRING DlgFind::GetCmbText(const UINT& cmb_id)
{
	//获取下拉框当前的文本
	StringUtils::T_STRING str;
	HWND hCmb = GetDlgItem(m_hDlgModeless,cmb_id);
	const int length = (int)::SendMessage(hCmb,WM_GETTEXTLENGTH,0,0);
	if (length < 1) {
		return str;
	}
	str.resize(length);
	::SendMessage(hCmb,WM_GETTEXT,length+1,LPARAM(str.c_str()));

	//清除文本空格
	if (str.length() < 1) {
		return str;
	}
	
	//将文本添加至下拉框列表中
	bool match = false;
	int lbtext_length(0);
	StringUtils::T_CHAR *pLBText = nullptr;
	const int count = (int)::SendMessage(hCmb,CB_GETCOUNT,0,0);
	for (int i=0; i<count; i++) {
		lbtext_length = (int)::SendMessage(hCmb,CB_GETLBTEXTLEN,i,0);
		pLBText = new StringUtils::T_CHAR[lbtext_length];
		::SendMessage(hCmb,CB_GETLBTEXT,i,LPARAM(pLBText));
		if (str.compare(pLBText) == 0) {
			match = TRUE;
			::SendMessage(hCmb,CB_SETCURSEL,i,0);
			break;
		}
	}
	if (!match) {
		int index = (int)::SendMessage(hCmb,CB_ADDSTRING,0,LPARAM(str.c_str()));
		::SendMessage(hCmb,CB_SETCURSEL,index,0);
	}
	return str;
}