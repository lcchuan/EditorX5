#include "stdafx.h"
#include "UIFrame.h"
#include "button.h"
#include "buttongroup.h"
#include "tab.h"
#include "../WinSDKUtils.h"
#include "../GdiplusHelp.h"

namespace lcc_direct_ui {
	////////////////////////////////////////////////////////////////////
	//UIFrame
	UIFrame UIFrame::m_instance;
	HBITMAP UIFrame::m_hEditorBackgroud = NULL;

	UIFrame::UIFrame()
		: m_colorBK(255,255, 255, 255), m_pTooltip(nullptr)

	{
		m_hWnd = 0;
		m_hCursorHand = NULL;

		m_pBrushForMessageBK = nullptr;
		m_pSfForMessage = nullptr;
	}

	UIFrame::~UIFrame()
	{
		for (size_t i = 0; i < m_uiobjs.size(); i++) {
			if (m_uiobjs[i]) {
				delete m_uiobjs[i];
			}
		}
		m_uiobjs.clear();
		m_pTooltip = nullptr; //会在m_uiobjs中被释放

		if (m_pBrushForMessageBK) {
			DELETE_GDIPLUS_OBJ(m_pBrushForMessageBK)
			m_pBrushForMessageBK = nullptr;
		}
		if (m_pSfForMessage) {
			DELETE_GDIPLUS_OBJ(m_pSfForMessage)
			m_pSfForMessage = nullptr;
		}
	}

	bool UIFrame::Create(const HINSTANCE& hInst, const HWND& hWnd, const RECT& rect)
	{
		m_hInst = hInst;
		m_hWnd = hWnd;

		UIBase::Create(hWnd, &rect);

		if (!m_pFont) {
			m_pFont = new Gdiplus::Font(m_fontFamilyName.c_str(), 8.6f, Gdiplus::FontStyle::FontStyleRegular);
		}

		m_hCursorSystem = NULL;
		m_hCursorCurrent = NULL;
		m_hCursorHand = ::LoadCursor(NULL, IDC_HAND);

		return true;
	}

	void UIFrame::ShowTip(UIBase* pUIObj, const std::wstring& tip, const POINT* pPt/*=nullptr*/)
	{
		if (!m_pTooltip) {
			m_pTooltip = new Tooltip();
			m_pTooltip->Create(m_hWnd);
			AddUIObject(m_pTooltip);
		}
		m_pTooltip->ShowTip(pUIObj, tip, pPt);
	}

	bool UIFrame::AddUIObject(UIBase *pUIObj)
	{
		m_uiobjs.push_back(pUIObj);
		return true;
	}

	bool UIFrame::DrawHint(Gdiplus::Graphics& graphic, const PAINTSTRUCT& ps)
	{
		if (m_messageText.length() < 1) {
			return false;
		}
		const intptr_t intersect = WinSDKUtils::IsIntersectRect(FromGdiplusRectF(m_messageRect), ps.rcPaint);
		if (0 == intersect) {
			return false;
		}

		if (!m_pBrushForMessageBK) {
			m_pBrushForMessageBK = new Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 255, 224));
		}

		const intptr_t radius = WinSDKUtils::GetAdaptationSizeForDPI(5);
		//填充背景
		GdiplusHelp::FillRoundRect(graphic, m_pBrushForMessageBK
			, m_messageRect
			, (INT)radius
		);
		//绘制提示信息
		Gdiplus::SolidBrush brushForText(Gdiplus::Color(255, 100, 0, 0));
		DrawText(graphic, &brushForText, m_pFont, m_messageText, m_messageRect, m_pSfForMessage);
		//绘制边框
		Gdiplus::Pen pen(Gdiplus::Color(255, 100, 0, 0));
		GdiplusHelp::DrawRoundRect(graphic, &pen, m_messageRect, (INT)radius);

		return -1 == intersect;
	}

	void UIFrame::Hint(const std::wstring& message, const POINT* pPt/*nullptr*/)
	{
		m_messageText = StringUtils::rtrim(message);
		if (m_messageText.length() < 1) {
			return;
		}

		if (!m_pSfForMessage) {
			m_pSfForMessage = new Gdiplus::StringFormat(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap
				| Gdiplus::StringFormatFlags::StringFormatFlagsNoClip
				| Gdiplus::StringFormatFlags::StringFormatFlagsNoFitBlackBox);
			m_pSfForMessage->SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
			m_pSfForMessage->SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
		}

		SIZE sz = GetTextSize(m_messageText, m_pFont, m_pSfForMessage);
		if (sz.cx < WinSDKUtils::GetAdaptationSizeForDPI(200)) {
			sz.cx = (LONG)WinSDKUtils::GetAdaptationSizeForDPI(200);
		}
		if (sz.cy < WinSDKUtils::GetAdaptationSizeForDPI(30)) {
			sz.cy = (LONG)WinSDKUtils::GetAdaptationSizeForDPI(30);
		}
		if (pPt) {
			m_messageRect.X = (Gdiplus::REAL)(pPt->x);
			m_messageRect.Y = (Gdiplus::REAL)(pPt->y);
		}
		else {
			m_messageRect.X = (Gdiplus::REAL)(m_width - sz.cx) / 2;
			m_messageRect.Y = (Gdiplus::REAL)WinSDKUtils::GetAdaptationSizeForDPI(7);
		}		
		m_messageRect.Width = (Gdiplus::REAL)(sz.cx + WinSDKUtils::GetAdaptationSizeForDPI(3));
		m_messageRect.Height = (Gdiplus::REAL)(sz.cy + WinSDKUtils::GetAdaptationSizeForDPI(3));
		RECT rect = FromGdiplusRectF(m_messageRect);
		::InvalidateRect(m_hWnd, &rect, FALSE);

		//隐藏tooltip
		HideTip(nullptr);

		::CreateThread(NULL, 0, DelayHideHintThread, this, 0, 0);
	}

	DWORD WINAPI UIFrame::DelayHideHintThread(LPVOID lpParam)
	{
		UIFrame* pUIFrame = (UIFrame*)lpParam;
		if (pUIFrame->m_messageText.length() < 1) {
			return 0;
		}		

		//延时自动关闭
		::Sleep(1500);

		const RECT rect = FromGdiplusRectF(pUIFrame->m_messageRect);

		//获取鼠标当前位置，如果鼠标在提示信息上，则不关闭提示信息
		POINT pt;
		while (true) {
			::GetCursorPos(&pt);
			::ScreenToClient(pUIFrame->m_hWnd, &pt);
			if (::PtInRect(&rect, pt)) {
				Sleep(500);
			}
			else {
				break;
			}
		}

		pUIFrame->m_messageText = TEXT("");
		::InvalidateRect(pUIFrame->m_hWnd, &rect, FALSE);
		return 0;
	}

	UIBase* UIFrame::GetUIObjectByID(const size_t& id) const
	{
		if (id > 0) {
			for (size_t i = 0; i < m_uiobjs.size(); i++) {
				if (m_uiobjs[i] && m_uiobjs[i]->GetId() == id) {
					return m_uiobjs[i];
				}						
			}
		}
		return nullptr;
	}

	bool UIFrame::Draw(const HDC& hdc, const PAINTSTRUCT& ps)
	{
		if (this == nullptr || !WinSDKUtils::IsIntersectRect(GetRect(),ps.rcPaint)) {
			return false;
		}

		//双缓冲绘制
		HDC hdcMem = ::CreateCompatibleDC(hdc);
		HBITMAP hBitmap = ::CreateCompatibleBitmap(hdc, (int)m_width, (int)m_height);
		HBITMAP hOldBmp = (HBITMAP)::SelectObject(hdcMem, hBitmap);
		::SetBkMode(hdcMem, TRANSPARENT);

		//GDIPlus对象
		Gdiplus::Graphics graphic(hdcMem);
		Gdiplus::SolidBrush brush(m_colorBK);
		Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0));
		graphic.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);//抗锯齿

		bool result(false);

		//填充背景色
		graphic.FillRectangle(&brush
			, ps.rcPaint.left - 1
			, ps.rcPaint.top - 1
			, WinSDKUtils::RectWidth(ps.rcPaint) + 1
			, WinSDKUtils::RectHeight(ps.rcPaint) + 1
		);			

		//分层绘制每一个UI对象，因为可能存在透明背景的情况，所以每一层的图形均绘制
		const size_t max_z = GetMaxZOrder();
		for (size_t z = 0; z <= max_z; z++) {
			for (size_t i = 0; i < m_uiobjs.size(); i++) {
				if (m_uiobjs[i]->GetZOrder() == z) {
					m_uiobjs[i]->Draw(graphic, ps);
				}
			}
		}

		//绘制上下边线
		graphic.DrawLine(&pen, (int)GetRectLeft(), (int)GetRectTop(), (int)GetRectRight(), (int)GetRectTop());
		graphic.DrawLine(&pen, (int)GetRectLeft(), (int)GetRectBottom() - 1, (int)GetRectRight(), (int)GetRectBottom() - 1);

		//绘制提示信息
		DrawHint(graphic, ps);

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
		return result;
	}

	bool UIFrame::OnMouseMove(const POINT& pt)
	{
		if (m_messageText.length() > 0) {
			return true;
		}
		const size_t max_z = GetMaxZOrder();
		for (intptr_t z = GetMaxZOrder(); z > -1; z--) {
			for (size_t i = 0; i<m_uiobjs.size(); i++) {
				if (m_uiobjs[i]->GetZOrder() == z && m_uiobjs[i]->OnMouseMove(pt)) {
					return true;
				}
			}
		}
		return false;
	}

	bool UIFrame::OnLButtonUp(const POINT& pt)
	{
		if (m_messageText.length() > 0) {
			return true;
		}
		const size_t max_z = GetMaxZOrder();
		for (intptr_t z = GetMaxZOrder(); z > -1; z--) {
			for (size_t i = 0; i<m_uiobjs.size(); i++) {
				if (m_uiobjs[i]->GetZOrder() == z && m_uiobjs[i]->OnLButtonUp(pt)) {
					return true;
				}
			}
		}
		return false;
	}

	bool UIFrame::OnLButtonDown(const POINT& pt)
	{
		if (m_messageText.length() > 0) {
			return true;
		}
		const size_t max_z = GetMaxZOrder();
		for (intptr_t z = GetMaxZOrder(); z > -1; z--) {
			for (size_t i = 0; i<m_uiobjs.size(); i++) {
				if (m_uiobjs[i]->GetZOrder() == z && m_uiobjs[i]->OnLButtonDown(pt)) {
					//const char * p = typeid(*m_uiobjs[i]).name();
					return true;
				}
			}
		}
		return false;
	}

	bool UIFrame::OnLButtonDblClk(const POINT& pt)
	{
		if (m_messageText.length() > 0) {
			return true;
		}
		const size_t max_z = GetMaxZOrder();
		for (intptr_t z = GetMaxZOrder(); z > -1; z--) {
			for (size_t i = 0; i<m_uiobjs.size(); i++) {
				if (m_uiobjs[i]->GetZOrder() == z && m_uiobjs[i]->OnLButtonDblClk(pt)) {
					return true;
				}
			}
		}
		return false;
	}

	bool UIFrame::OnRButtonDown(const POINT& pt)
	{
		if (m_messageText.length() > 0) {
			return true;
		}
		const size_t max_z = GetMaxZOrder();
		for (intptr_t z = GetMaxZOrder(); z > -1; z--) {
			for (size_t i = 0; i<m_uiobjs.size(); i++) {
				if (m_uiobjs[i]->GetZOrder() == z && m_uiobjs[i]->OnRButtonDown(pt)) {
					return true;
				}
			}
		}
		return false;
	}

	HBITMAP UIFrame::GrabEditorBackgroud()
	{
		RECT rect;
		::GetClientRect(m_hWnd, &rect);
		WinSDKUtils::ClientToScreen(m_hWnd, rect);
		rect.top += (int)m_height;

		const int width = WinSDKUtils::RectWidth(rect);
		const int height = WinSDKUtils::RectHeight(rect);

		HDC hDCScreen = ::CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
		HDC hDCTemp = ::CreateCompatibleDC(hDCScreen);
		HBITMAP hEditorBackgroud = ::CreateCompatibleBitmap(hDCScreen, width, height);
		HBITMAP oldBmp = (HBITMAP)::SelectObject(hDCTemp, hEditorBackgroud);
		::BitBlt(hDCTemp, 0, 0, width, height, hDCScreen, rect.left, rect.top, SRCCOPY);
		::SelectObject(hDCTemp, oldBmp);
		::DeleteDC(hDCTemp);
		::DeleteDC(hDCScreen);

		//CopyBitmapToClipBoard(hEditorBackgroud);

		return hEditorBackgroud;
	}

	size_t UIFrame::GetMaxZOrder() const
	{
		size_t max_z(0);
		for (intptr_t i = m_uiobjs.size() - 1; i > -1; i--) {
			if (m_uiobjs[i]->GetZOrder() > max_z) {
				max_z = m_uiobjs[i]->GetZOrder();
			}
		}
		return max_z;
	}
}
