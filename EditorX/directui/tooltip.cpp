#include "stdafx.h"
#include "tooltip.h"
#include "../StringUtils.h"
#include "../WinSDKUtils.h"
#include "../GdiplusHelp.h"

namespace lcc_direct_ui {
	Tooltip::Tooltip() {
		m_z = 1;

		m_pUIObj = nullptr;
		m_margin = 0;

		m_pStringFormat = new Gdiplus::StringFormat(Gdiplus::StringFormatFlags::StringFormatFlagsNoWrap
			| Gdiplus::StringFormatFlags::StringFormatFlagsNoClip
			| Gdiplus::StringFormatFlags::StringFormatFlagsNoFitBlackBox);
		m_pStringFormat->SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
		m_pStringFormat->SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
		m_pPenBorder = nullptr;
	}

	Tooltip::~Tooltip()
	{
		if (m_pStringFormat) {
			DELETE_GDIPLUS_OBJ(m_pStringFormat);
			m_pStringFormat = nullptr;
		}
		if (m_pPenBorder) {
			DELETE_GDIPLUS_OBJ(m_pPenBorder);
			m_pPenBorder = nullptr;
		}
	}

	bool Tooltip::Create(const HWND& hWnd)
	{
		UIBase::Create(hWnd, nullptr, 0);

		m_pPenBorder = new Gdiplus::Pen(Gdiplus::Color(255, 100, 100, 100));
		m_margin = WinSDKUtils::GetAdaptationSizeForDPI(3);
		return true;
	}

	void Tooltip::ShowTip(UIBase* pUIObj, const std::wstring& tip, const POINT* pPt/*=nullptr*/)
	{
		if (StringUtils::rtrim(tip).length() < 1 || pUIObj == nullptr) {
			HideTip(m_pUIObj);
			return;
		}
		if (pUIObj == m_pUIObj && (m_tip.compare(StringUtils::rtrim(tip)) == 0)) {
			return;
		}

		//重绘上一个tip的区域
		m_show = false;
		m_pUIObj = nullptr;
		Redraw();

		m_z = pUIObj->GetZOrder() + 1;
		m_pUIObj = pUIObj;
		m_tip = StringUtils::rtrim(tip);

		SIZE textSize = GetTextSize(m_tip,m_pFont, m_pStringFormat);
		m_width = textSize.cx+ (m_margin<<1);
		m_height = textSize.cy + (m_margin << 1);

		if (pPt) {
			m_x = pPt->x;
			m_y = pPt->y;
		}

		m_show = true;
		Redraw();
	}

	void Tooltip::HideTip(const UIBase* pUIObj)
	{
		if (pUIObj == nullptr || pUIObj == m_pUIObj) {
			m_pUIObj = nullptr;
			m_show = false;
			Redraw();
		}
	}

	bool Tooltip::Draw(Gdiplus::Graphics& graphic, const PAINTSTRUCT& ps)
	{
		if (!m_show) {
			return false;
		}

		const intptr_t intersect = WinSDKUtils::IsIntersectRect(GetRect(), ps.rcPaint);
		if (!intersect) {
			return false;
		}

		//擦除背景
		graphic.FillRectangle(m_pBrushBK
			, (INT)m_x
			, (INT)m_y
			, (INT)m_width
			, (INT)m_height);
		//绘制文本
		DrawText(graphic, m_pBrushForText, m_pFont, m_tip, GetRectF(), m_pStringFormat);
		//绘制边框
		INT x1((INT)GetRectLeft() + 1),y1((INT)GetRectTop() + 1);
		INT x2((INT)GetRectRight() - 1), y2((INT)GetRectBottom() - 1);
		graphic.DrawLine(m_pPenBorder,x1, y1, x2, y1);
		graphic.DrawLine(m_pPenBorder, x2, y1, x2, y2);
		graphic.DrawLine(m_pPenBorder, x2, y2, x1, y2);
		graphic.DrawLine(m_pPenBorder, x1, y2, x1, y1);

		return intersect == -1;
	}
}