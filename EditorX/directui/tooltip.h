#pragma once
#include "uibase.h"

namespace lcc_direct_ui {
	class Tooltip : public UIBase {
	public:
		Tooltip();
		virtual ~Tooltip();

		bool Create(const HWND& hWnd);
		inline UIBase* GetUIObj() const { return m_pUIObj; }

		void ShowTip(UIBase* pUIObj, const std::wstring& tip, const POINT* pPt=nullptr);
		void HideTip(const UIBase* pUIObj);

	protected:
		UIBase * m_pUIObj; //需要Tip的UI组件
		size_t m_margin;

		Gdiplus::StringFormat *m_pStringFormat;
		Gdiplus::Pen *m_pPenBorder;

		std::wstring m_tip;

		virtual bool Draw(Gdiplus::Graphics& graphic, const PAINTSTRUCT& ps);
	};
}