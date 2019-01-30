#pragma once
#include <vector>
#include "./directui/tab.h"
#include "./directui/UIFrame.h"
#include "MenuShorcut.h"
#include "MenuFileHistory.h"
#include "MenuSymbol.h"

//dor dropfiels
#include "Shellapi.h"
#pragma comment(lib,"Shell32.lib")

class ScintillaEdit;
class EditorXFrame
{
public:
	~EditorXFrame(void);

	//单例模式
	static EditorXFrame& GetInstance() {return m_instance;}
	static std::wstring GetMainWndClassName() {return MAINWND_CLASSNAME;}
	static std::wstring ParseCmdLine(LPCTSTR lpCmdLine);

	HINSTANCE GetAppInstance() const {return m_hInst;}
	HWND GetHWnd() const {return this==NULL ? NULL : m_hWnd;}

	//系统初始化
	BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

	int MessageBox(LPCTSTR lpText,LPCTSTR lpCaption=NULL,UINT uType=MB_OK);
	BOOL Confirm(LPCTSTR lpText,LPCTSTR lpCaption=NULL);

	void SetTitle(LPCTSTR title=NULL);
	BOOL OpenFile(LPCTSTR file=NULL);

	//更新应用标题栏
	void UpdateAppTitle(ScintillaEdit *pEditor = NULL);
	void UpdateTabTitle(ScintillaEdit *pEditor = NULL);
	//根据当前活动窗口更新菜单项的状态
	BOOL UpdateMainMenuStatus(ScintillaEdit *pEditor = NULL);

	//确保至少存在一个tab页
	void EnsureOneTabAtLeast();

	//新建一个编辑器tab页窗口，new的编辑器窗口对象会在m_tabBar.DeleteTab()中自动注销
	ScintillaEdit* CreateEditor(LPCTSTR title=NULL);
	ScintillaEdit* GetActiveEditor();
	/**
     * 通过编辑器的HWND获取相关信息
     * @param hWnd[in] 欲查找的编辑器的HWND
     * @param ppEditor[out] hWnd对应的编辑器的对象实例指针，用于输出
     * @param tabIndex[out] hWnd对应的编辑器的所属tab页的索引，用于输出
     * @return 搜索成功，返回TRUE
     */
	BOOL GetEditorFromHWND(const HWND& hWnd,ScintillaEdit** ppEditor,int& tabIndex);

	void ShowEditor(bool show);

protected:
	//UI对象ID定义
	enum :size_t {
		UIOBJ_ID_RDOGROUPLEXER = 1 //语法着色单选框组
		, UIOBJ_ID_RDOGROUPCODE    //文本编码单选框组
		, UIOBJ_ID_CHKGROUP        //复选框组："带BOM","自动换行","窗口最前"等
		, UIOBJ_ID_RIGHTBTNGROUP   //右侧按钮组的ID,“快捷”、“关于”等按钮
		, UIOBJ_ID_MAINTAB         //主tab
	};

	//单例模式
	static EditorXFrame m_instance;
	EditorXFrame(void);

	static const std::wstring MAINWND_CLASSNAME;
	
	HINSTANCE m_hInst; //应用的主实例句柄	
	HWND m_hWnd;       //应用的主窗口的句柄	
	HMODULE m_hScintillaDll; //编辑器DLL的句柄

	//directUI框架，不可注销该指针
	lcc_direct_ui::UIFrame *m_pUIFrame;
	lcc_direct_ui::Tab *m_pMainTab;

	//快捷方式菜单
	MenuShorcut m_menuShorcut;
	//文件历史菜单
	MenuFileHistory m_menuFileHistory;
	//符号选择
	MenuSymbol m_menuSymbol;

	void InitUIFrame();
	void OnResize(WPARAM wParam, LPARAM lParam);
    //@return TRUE-可以继续关闭，FALSE-不可以关闭
    BOOL OnClose();
	//程序框架的主消息响应
	static LRESULT CALLBACK	WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void GetEditorRect(RECT& rect);

	/**
	 * 处理菜单消息
	 * @param menu_id 菜单ID
	 * @return 可以识别该菜单ID并做了处理则返回true，否则返回false
	 */
	bool DealMenuMsg(int menu_id);

	//tab页关闭前的响应事件，详情请参见TabCtrl::m_pCallbackBeforeCloseTab的注解
	static intptr_t OnBeforeCloseTab(intptr_t index, intptr_t userData);
	//tab页关闭后的响应事件，详情请参见TabCtrl::m_pCallbackAfterCloseTab的注解
	static intptr_t OnAfterCloseTab(intptr_t index, intptr_t userData);
	//tab页切换的响应事件，详情请参见TabCtrl::m_pCallbackAfterChangeTab的注解
	static intptr_t OnChangeTab(intptr_t active, intptr_t preActive);
	//Tab双击事件的回掉函数，仅在双击空白区（非Tab项）时才会调用该函数，详情请参见TabCtrl::m_pCallbackDblClk的注解
	static intptr_t OnTabDblClk(intptr_t pPoint, intptr_t reserve);
	void OnDropFiles(HDROP hDropInfo);
};