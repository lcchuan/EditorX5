#include "StdAfx.h"
#include "EditorXView.h"

EditorXView::EditorXView(void)
{
	m_hWnd = NULL;
}


EditorXView::~EditorXView(void)
{
	if (::IsWindow(m_hWnd)) {
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

BOOL EditorXView::Create(HINSTANCE hInstance,HWND hParent,const RECT *p_rect/*=NULL*/)
{
	RECT rc = {0};
	if (p_rect) {
		::memcpy(&rc,p_rect,sizeof(RECT));
	}
	m_hWnd = ::CreateWindow(TEXT("Scintilla"), TEXT("editor"), WS_CHILD|WS_VISIBLE
		,rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top
		,hParent,NULL,hInstance,NULL);
	assert(m_hWnd);
	return m_hWnd != NULL;
}

BOOL EditorXView::MoveWindow(const RECT *p_rect,BOOL bRepaint/*=TRUE*/)
{
	if (!::IsWindow(m_hWnd)) {
		assert(false);
		return FALSE;
	}
	return ::MoveWindow(m_hWnd,p_rect->left,p_rect->top,p_rect->right,p_rect->bottom,bRepaint);
}
