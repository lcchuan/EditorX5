#pragma once
#include <string>

class BaseWnd
{
public:
	BaseWnd(void);
	virtual ~BaseWnd(void);

	virtual HWND GetHWnd() {return this==NULL ? NULL : m_hWnd;}
	HWND GetParent() {return m_hParent;}
	HINSTANCE GetHInstance() {return m_hInst;}

	virtual std::wstring GetToolTip() const {return TEXT("");}

	virtual BOOL Create(HINSTANCE hInstance,HWND hParent,const RECT *p_rect=NULL);
	virtual BOOL IsWindow() {return this == NULL ? FALSE : ::IsWindow(m_hWnd);}
	virtual BOOL MoveWindow(const RECT *p_rect,BOOL bRepaint = TRUE);
	virtual BOOL MoveWindow(int x,int y,int width,int height,BOOL bRepaint = TRUE);
	virtual HWND SetFocus() {return ::IsWindow(m_hWnd) ? ::SetFocus(m_hWnd) : NULL;}
	virtual BOOL ShowWindow(BOOL show) {return ::IsWindow(m_hWnd) ? ::ShowWindow(m_hWnd,show ? SW_SHOW : SW_HIDE) : FALSE;}
	virtual BOOL SetFont(HFONT hFont);

protected:
	HWND m_hWnd;
	HWND m_hParent;
	HINSTANCE m_hInst;
};

