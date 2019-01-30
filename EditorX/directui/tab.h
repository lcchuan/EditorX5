#pragma once
#include<string>
#include<vector>
#include "uibase.h"

namespace lcc_direct_ui {
	//tab页
	class Tab : public UIBase {
	public:
		struct ITEM {
			//唯一标识当前tab
			std::wstring uuid;

			std::wstring text;
			SIZE textSize;

			size_t state;  //Tab项状态:lcc_direct_ui::BTN_STATE_* 
			size_t closeBtnState;//关闭按钮的状态：0-正常显示，1-mouse hover ,100-隐藏

			std::wstring tip;
			SIZE tipSize;

			//用户自定义数据
			intptr_t userData;
		};
		typedef intptr_t(FUNC_CALLBACK)(intptr_t,intptr_t);

	public:
		Tab();
		virtual ~Tab();

		bool Create(const HWND& hWnd, const RECT* pRect = nullptr, const size_t& id = 0);
		const ITEM* AddItem(const std::wstring& text,const intptr_t& userData=0,bool redraw=true);
		//关闭某个tab项，关闭返回true,未关闭返回false
		bool CloseItem(const size_t& item, bool confirmBeforeClose=true);

		//tab项关闭前事件的回掉函数
		void SetCallbackBeforeCloseTab(FUNC_CALLBACK* pFunc) { m_pCallbackBeforeCloseTab = pFunc; }
		//tab项关闭后事件的回掉函数
		void SetCallbackAfterCloseTab(FUNC_CALLBACK* pFunc) { m_pCallbackAfterCloseTab = pFunc; }
		//tab项切换事件的回掉函数
		void SetCallbackAfterChangeTab(FUNC_CALLBACK* pFunc) { m_pCallbackAfterChangeTab = pFunc; }
		//Tab双击事件的回掉函数，仅在双击空白区（非Tab项）时才会调用该函数
		void SetCallbackDblClk(FUNC_CALLBACK* pFunc) { m_pCallbackDblClk = pFunc; }

		void SetActiveItem(const size_t& item);
		const ITEM* GetActiveItem() const;
		intptr_t GetActiveIndex() const;
		int HitTestForIndex(const POINT& pt);
		ITEM* HitTest(const POINT& pt);

		/*
		* 获得tab项的实际区域,GetItemAreaForDraw获得的是tab项的完整的梯形区域
		* @param points[out] 区域的顶点
		*/
		bool GetItemArea(const size_t& item, std::vector<POINT>& points);
		bool PtInItem(const POINT& pt, const size_t& item);

		inline size_t GetItemCount() const { return m_items.size(); }
		inline std::wstring GetItemTitle(const size_t& item) const { return (m_items.size() < item + 1) ? 0 : m_items[item]->text; }
		void SetItemTitle(const size_t& item, const std::wstring& title,bool redraw=true);
		inline std::wstring GetItemID(const size_t& item) const { return (m_items.size() < item + 1) ? 0 : m_items[item]->uuid; }
		intptr_t GetItemIndexByUserData(const intptr_t& userdata);
		intptr_t GetItemIndexByID(const std::wstring& id);
		ITEM* GetItemByID(const std::wstring& id);
		ITEM* GetItem(intptr_t index);

		//为某个tab项绑定用户自定义数据
		bool SetItemData(const size_t& item, intptr_t data);
		inline intptr_t GetItemData(const size_t& item) const { return (m_items.size() < item + 1) ? 0 : m_items[item]->userData; }
		intptr_t GetActiveItemData() const;

		inline bool IsActive(const size_t& item) const { return (m_items.size() < item + 1) ? false : IsActive(m_items[item]); }
		inline bool IsActive(const ITEM* pItem) const { return pItem->state & BTN_STATE_CHECKED; }
		inline bool IsHover(const size_t& item) const { return (m_items.size() < item + 1) ? false : (m_items[item]->state & BTN_STATE_HOVER); }

		virtual bool Draw(Gdiplus::Graphics& graphic, const PAINTSTRUCT& ps);
		virtual bool OnMouseMove(const POINT& pt);
		virtual bool OnLButtonDown(const POINT& pt);
		virtual bool OnLButtonDblClk(const POINT& pt);
		virtual bool OnRButtonDown(const POINT& pt);

	protected:
		//tab项完整区域的顶点个数，梯形，通常为4
		static const size_t ITEM_REGION_VERTEX_COUNT;

		std::vector<ITEM*> m_items;

		size_t m_itemWidth; //tab的宽度，及梯形的下边距
		size_t m_topWidth;  //梯形的上边距
		size_t m_itemHeight;//梯形的高度
		size_t m_bottomHeight; //底部区域的高度
		size_t m_leftMargin;   //左侧空白区的宽度
		size_t m_closeBtnRadius; //每个Tab项上关闭按钮的半径长度

		//该变量仅在DrawTabItem中被使用，在Create中被赋值，将其定义为类成员是为了提高计算效率
		intptr_t m_tempForDrawCloseBtn;
		Gdiplus::StringFormat *m_pStringFormat;

		Gdiplus::Color m_colorChecked;          //选中时的颜色
		Gdiplus::Color m_colorCheckedHover;     //选中时,鼠标在上移动时的颜色
		Gdiplus::Color m_colorDefault;          //非选中时的颜色
		Gdiplus::Color m_colorDefaultHover;     //非选中时,鼠标在上移动时的颜色
		Gdiplus::Color m_colorBorder;           //边框的颜色
		Gdiplus::Color m_colorCloseBtnHover;    //关闭按钮hover时的背景颜色

		/*
		 * 获得tab项完整的梯形区域,GetItemArea获得的是tab项的实际区域
		 * @param lpPoints[out] 数组个数必须为4（梯形）
		 */
		bool GetItemAreaForDraw(const size_t& item, Gdiplus::Point *lpPtGDIPlus);
		bool GetItemCloseBtnRect(const size_t& item, RECT& rect);
		void DrawTabItem(Gdiplus::Graphics& graphic
			, Gdiplus::Pen& penBorder
			, Gdiplus::SolidBrush& brush
			, Gdiplus::Point *lpPtGDIPlus
			, const RECT& rectCloseBtn
			, const ITEM& item);
		void SetItemTitle(ITEM* pItem, const std::wstring& title);

		/**
		* tab项关闭前事件的回掉函数
		* @param index 欲关闭的tab项的索引
		* @param userData 欲关闭的tab项绑定的用户数据
		* @return TRUE-可以继续关闭,FALSE-不可以关闭
		*/
		FUNC_CALLBACK* m_pCallbackBeforeCloseTab;
		/**
		* tab项关闭后事件的回掉函数
		* @param index 被关闭的tab项的索引,不可再利用该值调用m_items[index]，因为已被删除
		* @param userData 被关闭的tab项绑定的用户数据
		* @return 忽略返回值
		*/
		FUNC_CALLBACK* m_pCallbackAfterCloseTab;
		/**
		* tab项切换后事件的回掉函数
		* @param index 切换后的tab项的索引
		* @param preActive 切换前的活动tab项的索引
		* @return 忽略返回值
		*/
		FUNC_CALLBACK* m_pCallbackAfterChangeTab;
		/**
		 * Tab双击事件的回掉函数，仅在双击空白区（非Tab项）时才会调用该函数
		 * @param pPoint 鼠标点击的坐标
		 * @param reserve 忽略
		 * @return 忽略返回值
		 */
		FUNC_CALLBACK* m_pCallbackDblClk;
	};
}