#include "stdafx.h"
#include "MenuBase.h"
#include "WinSDKUtils.h"
#include "GdiplusHelp.h"
#include "EditorXFrame.h"

MenuBase::MenuBase() 
	: m_colorBK(255,255,255,255)
	, m_colorItemBKDefault(255, 255, 255, 255)
	, m_colorItemBKHover(255, 234, 243, 248)
{
	m_width = m_height = 0;
	m_itemHeight = 0;
	m_marginLeft = m_marginTop = 0;
	m_maxItemCount = 20;

	m_pFont = nullptr;
	m_pBrushForText = nullptr;
	m_pPenBorder = nullptr;
	m_pStringFormat = nullptr;

	m_redrawBackgroud = true;
}

MenuBase::~MenuBase()
{
	//如果先Gdiplus::GdiplusShutdown(m_gdiplusToken);再delete Gdiplus对象; 程序会崩溃。所以调用DELETE_GDIPLUS_OBJ宏
	if (m_pBrushForText) {
		DELETE_GDIPLUS_OBJ(m_pBrushForText);
		m_pBrushForText = nullptr;
	}
	if (m_pPenBorder) {
		DELETE_GDIPLUS_OBJ(m_pPenBorder);
		m_pPenBorder = NULL;
	}
	if (m_pFont) {
		DELETE_GDIPLUS_OBJ(m_pFont);
		m_pFont = nullptr;
	}
	if (m_pStringFormat) {
		DELETE_GDIPLUS_OBJ(m_pStringFormat);
		m_pStringFormat = nullptr;
	}

	for (size_t i = 0; i < m_items.size(); i++) {
		if (m_items[i]->hIcon) {
			::DeleteObject(m_items[i]->hIcon);
			m_items[i]->hIcon = NULL;
		}
		delete m_items[i];
	}
}

BOOL MenuBase::Create(HINSTANCE hInstance, HWND hParent)
{
	BaseWnd::Create(hInstance, hParent, NULL);
	m_itemHeight = WinSDKUtils::GetAdaptationSizeForDPI(26);
	m_marginLeft = WinSDKUtils::GetAdaptationSizeForDPI(3);
	m_marginTop = WinSDKUtils::GetAdaptationSizeForDPI(3);

	//注册窗口类
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_BYTEALIGNWINDOW | CS_DBLCLKS; //CS_HREDRAW|CS_VREDRAW
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hIconSm = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TEXT("lcc.EditorX5.Menu");
	RegisterClassEx(&wcex);

	//创建窗口
	m_hWnd = CreateWindowEx(0
		, wcex.lpszClassName, NULL
		, WS_POPUP
		,0,0,0,0
		, hParent, NULL, hInstance, NULL);
	if (!m_hWnd)
	{
		int err = GetLastError();
		assert(false);
		return FALSE;
	}

	//将窗口句柄与当前实例指针绑定
	::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

	//该句如果放到构造函数中，new会返回空指针，原因未知
	m_pFont = new Gdiplus::Font(TEXT("新宋体"), 7.0f, Gdiplus::FontStyle::FontStyleRegular);
	m_pBrushForText = new Gdiplus::SolidBrush(Gdiplus::Color(255, 0, 0, 0));
	m_pPenBorder = new Gdiplus::Pen(Gdiplus::Color(255, 200, 200, 200));
	m_pStringFormat = new Gdiplus::StringFormat(Gdiplus::StringFormatFlags::StringFormatFlagsNoClip
		| Gdiplus::StringFormatFlags::StringFormatFlagsNoFitBlackBox);
	m_pStringFormat->SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
	m_pStringFormat->SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	return TRUE;
}

MenuBase::ITEM* MenuBase::AddItem(const std::wstring& text, const size_t& id, HICON hIcon)
{
	ITEM *pItem = new ITEM();
	pItem->text = text;
	pItem->id = id;
	pItem->hIcon = hIcon;
	pItem->state = 0;
	pItem->iconSize.cx = pItem->iconSize.cy = (LONG)WinSDKUtils::GetAdaptationSizeForDPI(20);

	//计算文本的长度
	HDC hdc = ::GetDC(m_hWnd);
	Gdiplus::Graphics graphic(hdc);
	pItem->textSize = lcc_direct_ui::UIBase::GetTextSize(graphic,text,m_pFont,m_pStringFormat);
	::ReleaseDC(m_hWnd, hdc);

	m_items.push_back(pItem);
	return pItem;
}

LRESULT CALLBACK MenuBase::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	POINT pt;
	MenuBase *pMe = nullptr;

	switch (message)
	{
	case WM_PAINT:
		pMe = (MenuBase*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
		hdc = BeginPaint(hWnd, &ps);
		pMe->OnPaint(hdc, ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_ACTIVATE: //失去焦点则注销对话框
		if (WA_INACTIVE == LOWORD(wParam)) {
			::ShowWindow(hWnd,SW_HIDE);
		}

		break;
	case WM_MOUSEMOVE:
		pMe = (MenuBase*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		pMe->OnMouseMove(pt);
		break;
	case WM_LBUTTONDOWN:
		pMe = (MenuBase*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		pMe->OnLButtonDown(pt);
		break;
	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void MenuBase::OnMouseMove(const POINT& pt)
{
	RECT rcItem;
	for (int i = 0; i < m_items.size(); i++) {
		if (HitTest(pt,i,&rcItem)) {
			if (!IsHover(m_items[i])) {
				m_items[i]->state |= lcc_direct_ui::BTN_STATE_HOVER;
				::InvalidateRect(m_hWnd, &rcItem, FALSE);
			}
		}
		else if (IsHover(m_items[i])) {
			m_items[i]->state &= ~lcc_direct_ui::BTN_STATE_HOVER;
			GetItemRect(i, &rcItem);
			::InvalidateRect(m_hWnd, &rcItem, FALSE);
		}
	}
}

void MenuBase::OnPaint(HDC& hdc, PAINTSTRUCT& ps)
{
	//双缓冲绘制
	HDC hdcMem = ::CreateCompatibleDC(hdc);
	HBITMAP hBitmap = ::CreateCompatibleBitmap(hdc, (int)m_width+1, (int)m_height+1);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hdcMem, hBitmap);
	::SetBkMode(hdcMem, TRANSPARENT);

	//GDIPlus对象
	Gdiplus::Graphics graphic(hdcMem);
	graphic.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);//抗锯齿

	if (m_redrawBackgroud) {
		ps.fErase = TRUE;
		m_redrawBackgroud = false;
	}

	//绘制菜单
	ps.hdc = hdcMem;
	Draw(graphic,ps);
	ps.hdc = hdc;

	//将内存DC中的内容复制到hdc中
	::BitBlt(hdc
		, ps.rcPaint.left, ps.rcPaint.top, WinSDKUtils::RectWidth(ps.rcPaint), WinSDKUtils::RectHeight(ps.rcPaint)
		, hdcMem
		, ps.rcPaint.left, ps.rcPaint.top
		, SRCCOPY
	);
	::SelectObject(hdcMem, hOldBmp);
	::DeleteDC(hdcMem);
	::DeleteObject(hBitmap);
}

void MenuBase::Draw(Gdiplus::Graphics& graphic, PAINTSTRUCT& ps)
{
	//填充背景色
	if (ps.fErase) {
		Gdiplus::SolidBrush brush(m_colorBK);
		graphic.FillRectangle(&brush
			, ps.rcPaint.left - 1
			, ps.rcPaint.top - 1
			, WinSDKUtils::RectWidth(ps.rcPaint) + 1
			, WinSDKUtils::RectHeight(ps.rcPaint) + 1
		);
	}

	//绘制菜单
	Gdiplus::SolidBrush brushItemBK(m_colorItemBKDefault);
	intptr_t intersect(0);
	RECT rcItem;
	for (int i = 0; i < m_items.size(); i++) {
		GetItemRect(i, &rcItem);
		intersect = WinSDKUtils::IsIntersectRect(rcItem, ps.rcPaint);
		if (!intersect) {
			continue;
		}

		if (m_items[i]->state & lcc_direct_ui::BTN_STATE_HOVER) {
			brushItemBK.SetColor(m_colorItemBKHover);
		}
		else {
			brushItemBK.SetColor(m_colorItemBKDefault);
		}
		DrawItem(graphic, &brushItemBK, m_items[i], rcItem,ps.hdc);

		if (-1 == intersect) {
			break;
		}
	}

	//绘制整体边框
	graphic.DrawLine(m_pPenBorder, 0, 0, (int)(m_width - 1), 0);
	graphic.DrawLine(m_pPenBorder, (int)(m_width - 1), 0, (int)(m_width - 1), (int)(m_height - 1));
	graphic.DrawLine(m_pPenBorder, (int)(m_width - 1), (int)(m_height - 1), 0, (int)(m_height - 1));
	graphic.DrawLine(m_pPenBorder, 0, (int)(m_height - 1), 0, 0);
}

//@param pt 菜单客户区坐标
const MenuBase::ITEM* MenuBase::HitTest(const POINT& pt) 
{
	const intptr_t index = HitTestForIndex(pt);
	return index>-1 ? m_items[index] : nullptr;
}

intptr_t MenuBase::HitTestForIndex(const POINT& pt) const
{
	RECT rcItem = { 0 };
	for (size_t i = 0; i < m_items.size(); i++) {
		GetItemRect(i, &rcItem);
		if (::PtInRect(&rcItem, pt)) {
			return i;
		}
	}
	return -1;
}

bool MenuBase::HitTest(const POINT& pt, const int& item, RECT *pItemRect/*= nullptr*/)
{
	RECT rcItem = { 0};
	if (!GetItemRect(item, &rcItem)) {
		assert(false);
		return false;
	}

	if (::PtInRect(&rcItem, pt)) {
		if (pItemRect) {
			*pItemRect = rcItem;
		}
		return true;
	}
	else {
		return false;
	}
}

bool MenuBase::GetItemRect(const size_t& item, RECT *pItemRect) const
{
	if (m_items.size() - 1 < item) {
		assert(false);
		return false;
	}
	pItemRect->left = (LONG)m_marginLeft;
	pItemRect->top = (LONG)(m_marginTop+item * m_itemHeight);
	pItemRect->right = (LONG)(pItemRect->left+m_width- (m_marginLeft<<1));
	pItemRect->bottom = (LONG)(pItemRect->top + m_itemHeight);
	return true;
}

void MenuBase::OnLButtonDown(const POINT& pt)
{
	if (::IsWindow(m_hParent)) {
		const ITEM* pItem = HitTest(pt);
		if (pItem) {
			::PostMessage(m_hParent, WM_COMMAND, MAKELONG(pItem->id, 0), 0);
		}		
	}	
}

void MenuBase::Popup()
{
	HWND hParent = ::GetParent(m_hWnd);
	RECT rcParent;
	::GetClientRect(hParent, &rcParent);

	intptr_t x(0),y(WinSDKUtils::GetAdaptationSizeForDPI(140));
	if (m_width < rcParent.right - rcParent.left) {
		x += (rcParent.right - rcParent.left - m_width) / 2;
	}

	Popup(x, y);
}

//x,y 父窗口的窗口坐标
void MenuBase::Popup(intptr_t x, intptr_t y) {
	if (!::IsWindow(m_hWnd)) {
		assert(false);
		return;
	}

	if (m_items.size() < 1) {
		EditorXFrame::GetInstance().MessageBox(TEXT("暂无项目被加载"));
		return;
	}

	m_redrawBackgroud = true;
	POINT pt{ (LONG)x,(LONG)y };
	::ClientToScreen(::GetParent(m_hWnd),&pt);
	::SetWindowPos(m_hWnd, HWND_TOP, pt.x, pt.y, (LONG)m_width, (LONG)m_height, SWP_SHOWWINDOW);
}
