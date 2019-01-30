#pragma once
#include "uibase.h"
#include "button.h"

namespace lcc_direct_ui {
	//按钮组
	class ButtonGroup : public UIBase {
	public:
		ButtonGroup();
		~ButtonGroup(void) {
			for (size_t i = 0; i < m_buttons.size(); i++) {
				delete m_buttons[i];
			}
		}

		bool Create(const HWND& hWnd, const RECT& rect,const bool& checkbox,const bool& multi_selected
			,const size_t& btnWidth, const size_t& btnHeight, const size_t& btnSpace, const size_t& id = 0);
		int AddButton(const StringUtils::T_STRING& text, const size_t& id,const size_t& width=0,bool checked=false);
		int AddButton(const StringUtils::T_STRING& image, const StringUtils::T_STRING& tip, const size_t& id, bool checked = false);
		Button* HitTest(const POINT& pt);

		void ModifyBtnStyle(const Button::BTN_STYLE& style);
		void ModifyBtnColor(const Gdiplus::Color& colorChecked
			, const Gdiplus::Color& colorCheckedHover
			, const Gdiplus::Color& colorDefault
			, const Gdiplus::Color& colorDefaultHover);

		virtual void Offset(const intptr_t& x, const intptr_t& y);

		void SetChecked(const int& index,const bool& checked);
		void SetCheckedById(const size_t& id, const bool& checked);

		//@return true-表明不需要其它绘制函数处理了，因为ps.rcPaint与某个按钮完全相等
		virtual bool Draw(Gdiplus::Graphics& graphic, const PAINTSTRUCT& ps);
		virtual bool OnMouseMove(const POINT& pt);
		virtual bool OnLButtonDown(const POINT& pt);
		virtual bool OnLButtonUp(const POINT& pt);

	protected:
		//m_checkbox为false时，忽略该参数
		bool m_multiSelected;
		//是否为单选组合或多选组合框
		bool m_checkbox;
		
		std::vector<Button*> m_buttons;

		size_t m_btnWidth, m_btnHeight, m_btnSpace;
	};
}