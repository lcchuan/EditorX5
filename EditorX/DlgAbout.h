#pragma once

//"关于"对话框
class DlgAbout
{
public:
	~DlgAbout();
	static DlgAbout& GetInstance();

	//弹出无模式对话框
	void ShowInModeless();

protected:
	static DlgAbout m_instance;
	DlgAbout();

	HWND m_hDlgModeless;
	RECT m_rcDlg;
	HBRUSH m_brushBK;

	static INT_PTR CALLBACK OnDlgMsg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};

