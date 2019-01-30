#include "stdafx.h"
#include "resource.h"
#include "DlgAbout.h"
#include "EditorXFrame.h"
#include "ScintillaEdit.h"
#include "WinSDKUtils.h"

DlgAbout DlgAbout::m_instance;
DlgAbout::DlgAbout()
{
	m_hDlgModeless = NULL;
	m_brushBK = NULL;
}


DlgAbout::~DlgAbout()
{
	if (m_brushBK) {
		::DeleteObject(m_brushBK);
		m_brushBK = NULL;
	}
}

void DlgAbout::ShowInModeless()
{
	AnimateWindow(m_hDlgModeless, 200, AW_CENTER);//动画方式打开窗口
}

DlgAbout& DlgAbout::GetInstance()
{
	if (!m_instance.m_hDlgModeless) {
		m_instance.m_hDlgModeless = ::CreateDialogParam(EditorXFrame::GetInstance().GetAppInstance()
			, MAKEINTRESOURCE(IDD_ABOUTBOX)
			, EditorXFrame::GetInstance().GetHWnd()
			, OnDlgMsg
			, LPARAM(&m_instance));
		::SetWindowLongPtr(m_instance.m_hDlgModeless, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&m_instance));
	}

	return m_instance;
}

INT_PTR CALLBACK DlgAbout::OnDlgMsg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	DlgAbout *pMe = nullptr;

	HDC hdc(NULL);
	RECT rcDlg, rcFrame;
	switch (message)
	{
	case WM_INITDIALOG:
		pMe = (DlgAbout *)lParam;
		pMe->m_brushBK = ::CreateSolidBrush(RGB(255, 255, 255));

		//对话框居中显示
		::GetWindowRect(hDlg, &rcDlg);
		::GetWindowRect(EditorXFrame::GetInstance().GetHWnd(), &rcFrame);
		::SetWindowPos(hDlg, HWND_TOP
			, ((rcFrame.right - rcFrame.left) - (rcDlg.right - rcDlg.left)) / 2 + rcFrame.left
			, (int)(rcFrame.top + WinSDKUtils::GetAdaptationSizeForDPI(140))
			, 0, 0, SWP_NOSIZE);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;

	case WM_ACTIVATE: //失去焦点则注销对话框
		if (WA_INACTIVE == LOWORD(wParam)) {
			::DestroyWindow(m_instance.m_hDlgModeless);
			m_instance.m_hDlgModeless = NULL;
			EditorXFrame::GetInstance().GetActiveEditor()->SetFocus();
		}		

		break;

	case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC:
		pMe = (DlgAbout *)::GetWindowLongPtr(hDlg, GWLP_USERDATA);
		return (INT_PTR)(pMe->m_brushBK);
	}
	return (INT_PTR)FALSE;
}
