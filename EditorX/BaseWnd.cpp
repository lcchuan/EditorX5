#include "StdAfx.h"
#include "BaseWnd.h"


BaseWnd::BaseWnd(void)
{
	m_hWnd = NULL;
	m_hParent = NULL;
	m_hInst = NULL;
}


BaseWnd::~BaseWnd(void)
{
	if (::IsWindow(m_hWnd)) {
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

BOOL BaseWnd::Create(HINSTANCE hInstance,HWND hParent,const RECT *p_rect/*=NULL*/) {
	m_hInst = hInstance;
	m_hParent = hParent;
	return TRUE;
}

BOOL BaseWnd::MoveWindow(const RECT *p_rect,BOOL bRepaint/*=TRUE*/)
{
	return MoveWindow(p_rect->left,p_rect->top,p_rect->right-p_rect->left,p_rect->bottom-p_rect->top,bRepaint);
}

BOOL BaseWnd::MoveWindow(int x,int y,int width,int height,BOOL bRepaint/*=TRUE*/) {
	if (!IsWindow()) {
		assert(false);
		return FALSE;
	}
	return ::MoveWindow(m_hWnd,x,y,width,height,bRepaint);
}

BOOL BaseWnd::SetFont(HFONT hFont)
{
	if (!IsWindow()) {
		assert(false);
		return FALSE;
	}
	::SendMessage(m_hWnd, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), 0);	
	return TRUE;
}
