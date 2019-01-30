#pragma once
#include "StringUtils.h"

//搜索与替换对话框
class DlgFind
{
public:	
	~DlgFind(void);

	static DlgFind& GetInstance() ;

	//弹出无模式对话框
	void ShowInModeless(const StringUtils::T_STRING& search_text=TEXT(""));

	BOOL Find();
	BOOL Replace(bool replace_all=false);

protected:
	static DlgFind m_instance;
	DlgFind(void);

	HWND m_hDlgModeless;
	RECT m_rcDlg;
	HICON m_hIcon;

	HFONT m_hFontHint;
	RECT m_rcHint;
	StringUtils::T_STRING m_strHint;

	//返回下拉框的当前文本
	StringUtils::T_STRING GetCmbText(const UINT& cmb_id);
	static INT_PTR CALLBACK OnDlgMsg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};

