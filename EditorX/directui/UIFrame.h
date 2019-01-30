#pragma once
#include<vector>
#include "uibase.h"
#include "tooltip.h"

/* 以DirectUI方式生成的界面组件，所有组件以UIBase为基类,利用UIFrame管理所有界面组件
 * 利用Gdiplu绘制界面
 * 故在程序启动时需调用lcc_direct_ui::UIBase::LoadGdiplus以启动Gdiplus环境
 * 程序关闭时调用lcc_direct_ui::UIBase::UnloadGdiplus以关闭Gdiplus环境
 * 李长川 201802
 */
namespace lcc_direct_ui {
	/**
	 * UI框架，用于集成各directUI组件
	 * 利用GDIPlus进行绘制，所以需要在程序初始化时调用Gdiplus::GdiplusStartup与GdiplusShutdown
	 */
	class UIFrame : public UIBase
	{
	public:
		virtual ~UIFrame();

		static inline UIFrame& GetInstance() { return m_instance; }
		bool Create(const HINSTANCE& hInst,const HWND& hWnd, const RECT& rect);
		bool AddUIObject(UIBase *pUIObj);

		//如果pPt为null，则默认居中显示
		void Hint(const std::wstring& message,const POINT* pPt=nullptr);

		void ShowTip(UIBase* pUIObj, const std::wstring& tip, const POINT* pPt = nullptr);
		inline void HideTip(const UIBase* pUIObj) { if (m_pTooltip) m_pTooltip->HideTip(pUIObj); }

		//获取所有UI对象中的最大的Z轴位置
		size_t GetMaxZOrder() const;

		UIBase* GetUIObjectByID(const size_t& id) const;

		//@return true-表明不需要其它绘制函数处理了
		bool Draw(const HDC& hdc, const PAINTSTRUCT& ps);
		bool OnMouseMove(const POINT& pt);
		bool OnLButtonDown(const POINT& pt);
		bool OnLButtonDblClk(const POINT& pt);
		bool OnLButtonUp(const POINT& pt);
		bool OnRButtonDown(const POINT& pt);

		HBITMAP GrabEditorBackgroud();

	protected:
		friend class EditorXFrame;
		static UIFrame m_instance;
		UIFrame();

		static HBITMAP m_hEditorBackgroud;

		HCURSOR m_hCursorSystem;
		HCURSOR m_hCursorCurrent;
		HCURSOR m_hCursorHand;

		HINSTANCE m_hInst; //应用的主实例句柄
		HWND m_hWnd;
		Gdiplus::Color m_colorBK; //背景色

		/*
		 * 存储所有的UI对象
		 */
		std::vector<UIBase*> m_uiobjs;
		Tooltip *m_pTooltip;

		//for 提示信息
		std::wstring m_messageText;
		Gdiplus::RectF m_messageRect;
		Gdiplus::StringFormat *m_pSfForMessage;
		Gdiplus::Brush *m_pBrushForMessageBK;
		bool DrawHint(Gdiplus::Graphics& graphic, const PAINTSTRUCT& ps);
		//延时隐藏提示信息
		static DWORD WINAPI DelayHideHintThread(LPVOID lpParam);

	private:
		//禁止调用父类的函数
		bool Create(const HWND& hWnd, const RECT* pRect = nullptr, const size_t& id = 0) { return false; }
		bool Draw(Gdiplus::Graphics& graphic, const PAINTSTRUCT& ps) { return false; }
	};

};

