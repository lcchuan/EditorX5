#include "stdafx.h"
#include "buttongroup.h"
#include "../WinSDKUtils.h"

namespace lcc_direct_ui {
	ButtonGroup::ButtonGroup() : UIBase()
	{
		m_multiSelected = false;
		m_btnWidth = 0;
		m_btnHeight = 0;
		m_btnSpace = 0;
	}

	bool ButtonGroup::Create(const HWND& hWnd, const RECT& rect, const bool& checkbox, const bool& multi_selected
		, const size_t& btnWidth, const size_t& btnHeight, const size_t& btnSpace, const size_t& id/*=0*/)
	{
		UIBase::Create(hWnd, &rect, id);

		m_checkbox = checkbox;
		m_multiSelected = multi_selected;

		m_btnWidth = btnWidth;
		m_btnHeight = btnHeight;
		m_btnSpace = btnSpace;
		m_height = m_btnSpace + m_btnHeight + m_btnSpace;
		return true;
	}

	void ButtonGroup::ModifyBtnColor(const Gdiplus::Color& colorChecked
		, const Gdiplus::Color& colorCheckedHover
		, const Gdiplus::Color& colorDefault
		, const Gdiplus::Color& colorDefaultHover)
	{
		for (int i = 0; i < m_buttons.size(); i++) {
			m_buttons[i]->SetColor(colorChecked, colorCheckedHover, colorDefault, colorDefaultHover);
		}
	}

	void ButtonGroup::ModifyBtnStyle(const Button::BTN_STYLE& style)
	{
		for (size_t i = 0; i < m_buttons.size(); i++) {
			m_buttons[i]->SetStyle(style);
		}
	}

	int ButtonGroup::AddButton(const StringUtils::T_STRING& image
		, const StringUtils::T_STRING& tip
		, const size_t& id
		, bool checked/*= false*/)
	{
		const Button *pBefore = m_buttons.size() > 0 ? m_buttons[m_buttons.size() - 1] : nullptr;

		RECT rect;
		if (pBefore) {
			rect.left = (LONG)(pBefore->GetRectRight() + m_btnSpace);
			rect.top = (LONG)pBefore->GetRectTop();
		}
		else {
			rect.left = (LONG)(GetRectLeft() + m_btnSpace);
			rect.top = (LONG)(GetRectTop() + m_btnSpace);
		}
		rect.right = (LONG)(rect.left + m_btnWidth);
		rect.bottom = (LONG)(rect.top + m_btnHeight);

		Button *pBtn = new Button();
		pBtn->Create(m_hWnd, &rect, image,tip, id);
		pBtn->SetChecked(checked, false);
		m_buttons.push_back(pBtn);

		m_width += pBtn->GetRectWidth() + m_btnSpace;
		return (int)(m_buttons.size()) - 1;
	}

	int ButtonGroup::AddButton(const StringUtils::T_STRING& text, const size_t& id, const size_t& width/*=0*/, bool checked/*=false*/)
	{
		const Button *pBefore = m_buttons.size() > 0 ? m_buttons[m_buttons.size() - 1] : nullptr;

		RECT rect;
		if (pBefore) {
			rect.left = (LONG)(pBefore->GetRectRight() + m_btnSpace);
			rect.top = (LONG)pBefore->GetRectTop();
		}
		else {
			rect.left = (LONG)(GetRectLeft() + m_btnSpace);
			rect.top = (LONG)(GetRectTop() + m_btnSpace);
		}
		rect.right = (LONG)(rect.left + (width < 1 ? m_btnWidth : width));
		rect.bottom = (LONG)(rect.top + m_btnHeight);
 
		Button *pBtn = new Button();
		pBtn->Create(m_hWnd, rect, text, id);
		pBtn->SetChecked(checked, false);
		if (m_checkbox) {// 设置按钮样式
			pBtn->SetStyle(m_multiSelected ? Button::BTN_STYLE::BS_ELLIPSE : Button::BTN_STYLE::BS_ONLYTEXT);
		}
		if (m_checkbox && !m_multiSelected) {//设置按钮字体带下划线
			pBtn->ModifyFont(Gdiplus::FontStyle::FontStyleUnderline);
		}
		m_buttons.push_back(pBtn);

		m_width += pBtn->GetRectWidth() + m_btnSpace;
		return (int)(m_buttons.size()) - 1;
	}

	void ButtonGroup::SetChecked(const int& index, const bool& checked)
	{
		if (!m_checkbox) {
			assert(false);
			return;
		}
		if (m_multiSelected) {
			if (0 <= index && index < m_buttons.size()) {
				if (checked && m_buttons[index]->IsChecked()) {
					return;
				}
				if (!checked && !(m_buttons[index]->IsChecked())) {
					return;
				}
				m_buttons[index]->SetChecked(checked);
			}
		}
		else if (checked) {
			for (int i = 0; i < m_buttons.size(); i++) {
				if (i == index) {
					if (m_buttons[i]->IsChecked()) {
						return;
					}
					else {
						m_buttons[i]->SetChecked(true);
					}
				}
				else if (m_buttons[i]->IsChecked()) {
					m_buttons[i]->SetChecked(false);
				}
			}
		}
		else {
			if (0 <= index && index < m_buttons.size() && m_buttons[index]->IsChecked()) {
				m_buttons[index]->SetChecked(false);
			}
		}
	}

	void ButtonGroup::SetCheckedById(const size_t& id, const bool& checked)
	{
		if (!m_checkbox) {
			assert(false);
			return;
		}
		if (m_multiSelected) {
			for (int i = 0; i < m_buttons.size(); i++) {
				if (m_buttons[i]->GetId() == id) {
					if (checked && m_buttons[i]->IsChecked()) {
						break;
					}
					if (!checked && !(m_buttons[i]->IsChecked())) {
						break;
					}
					m_buttons[i]->SetChecked(checked);
				}
			}
		}
		else {
			for (int i = 0; i < m_buttons.size(); i++) {
				if (m_buttons[i]->GetId() == id) {
					if (checked && m_buttons[i]->IsChecked()) {
						break;
					}
					if (!checked && !(m_buttons[i]->IsChecked())) {
						break;
					}
					m_buttons[i]->SetChecked(checked);
				}
				else if (checked && m_buttons[i]->IsChecked()) {
					m_buttons[i]->SetChecked(false);
				}
			}
		}
	}

	bool ButtonGroup::Draw(Gdiplus::Graphics& graphic, const PAINTSTRUCT& ps)
	{
		const intptr_t intersect = WinSDKUtils::IsIntersectRect(GetRect(), ps.rcPaint);
		if (!intersect) {
			return false;
		}

		for (int i = 0; i < m_buttons.size(); i++) {
			if (m_buttons[i]->Draw(graphic, ps)) {
				return true;
			}
		}
		return intersect == -1;
	}

	bool ButtonGroup::OnMouseMove(const POINT& pt)
	{
		bool match(false);
		for (int i = 0; i < m_buttons.size(); i++) {
			if (m_buttons[i]->OnMouseMove(pt)) {
				match = true;
			}
		}
		return match;
	}

	bool ButtonGroup::OnLButtonDown(const POINT& pt)
	{
		if (!PtInArea(pt)) {
			return false;
		}

		Button *pBtn = HitTest(pt);
		if (pBtn) {
			if (m_checkbox) {
				SetCheckedById(pBtn->GetId(), m_multiSelected ? !(pBtn->IsChecked()) : true);
				::PostMessage(m_hWnd, WM_COMMAND, MAKELONG(pBtn->GetId(), 0), 0);
			}
			else {
				pBtn->SetChecked(true);
			}
		}
		return true;
	}

	bool ButtonGroup::OnLButtonUp(const POINT& pt)
	{
		if (!PtInArea(pt)) {
			return false;
		}
		if (m_checkbox) {
			return true;
		}

		Button *pBtn = HitTest(pt);
		if (pBtn) {
			pBtn->OnLButtonUp(pt);
		}
		return true;
	}

	Button* ButtonGroup::HitTest(const POINT& pt)
	{
		for (int i = 0; i < m_buttons.size(); i++) {
			if (m_buttons[i]->PtInArea(pt)) {
				return m_buttons[i];
			}
		}
		return nullptr;
	}

	void ButtonGroup::Offset(const intptr_t& x, const intptr_t& y)
	{
		if (x == 0 && y == 0) {
			return;
		}
		UIBase::Offset(x,y);

		for (int i = 0; i < m_buttons.size(); i++) {
			m_buttons[i]->Offset(x,y);
		}
	}
}