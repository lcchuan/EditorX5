#pragma once
#include "uibase.h"
#include "../StringUtils.h"

namespace lcc_direct_ui {
	/////////////////////////////////////////////////////////////////
	//按钮
	class Button : public UIBase {
	public:
		enum class BTN_STYLE : unsigned char {
			BS_DEFAULT = 0//按钮边框是圆角矩形
			, BS_ONLYTEXT //仅显示文本，不显示按钮背景及边框
			, BS_ELLIPSE  //按钮边框是椭圆
		};

		Button();
		virtual ~Button();

		bool Create(const HWND& hWnd, const RECT& rect, const StringUtils::T_STRING& text, const size_t& id = 0);
		bool Create(const HWND& hWnd
			, const RECT* pRect
			, const StringUtils::T_STRING& image
			, const StringUtils::T_STRING& tip
			, const size_t& id = 0);
		void SetColor(const Gdiplus::Color& colorChecked
			, const Gdiplus::Color& colorCheckedHover
			, const Gdiplus::Color& colorDefault
			, const Gdiplus::Color& colorDefaultHover) {
			m_colorChecked = colorChecked;
			m_colorCheckedHover = colorCheckedHover;
			m_colorDefault = colorDefault;
			m_colorDefaultHover = colorDefaultHover;
		}

		inline StringUtils::T_STRING GetText() const { return m_text; }
		inline bool IsChecked() const { return m_state & BTN_STATE_CHECKED; }
		inline void SetChecked(bool checked,bool redraw=true) {
			m_state = checked ? (m_state|BTN_STATE_CHECKED) : (m_state & ~BTN_STATE_CHECKED);
			if (redraw) {
				Redraw();
			}
		}
		inline bool IsHover() const { return m_state & BTN_STATE_HOVER; }
		inline void SetHover(bool hover) {
			m_state = hover ? (m_state|BTN_STATE_HOVER) : (m_state & ~BTN_STATE_HOVER);
			Redraw();
		}

		void SetStyle(const BTN_STYLE& style) { m_style = style; }
		
		//@return true-表明不再需要其它消息响应函数处理了
		virtual bool Draw(Gdiplus::Graphics& graphic, const PAINTSTRUCT& ps);
		virtual bool OnMouseMove(const POINT& pt);
		virtual bool OnLButtonDown(const POINT& pt);
		virtual bool OnLButtonUp(const POINT& pt);
	protected:
		Gdiplus::Image *m_pImage;
		SIZE m_imageShowSize; //图片的显示大小，默认为WinSDKUtils::GetAdaptationSizeForDPI(32)
		StringUtils::T_STRING m_text;
		StringUtils::T_STRING m_tip;
		BTN_STYLE m_style;

		//0 BTN_STATE_CHECKED BTN_STATE_HOVER
		size_t m_state;

		Gdiplus::Color m_colorChecked;      //选中时的颜色
		Gdiplus::Color m_colorCheckedHover; //选中时,鼠标在上移动时的颜色
		Gdiplus::Color m_colorDefault;      //非选中时的颜色
		Gdiplus::Color m_colorDefaultHover; //非选中时,鼠标在上移动时的颜色
	};
}