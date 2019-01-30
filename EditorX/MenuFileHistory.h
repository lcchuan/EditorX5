#pragma once
#include <string>
#include "MenuBase.h"

/*历史文件记录*/
class MenuFileHistory : public MenuBase
{
public:
	MenuFileHistory();
	virtual ~MenuFileHistory();

	virtual BOOL Create(HINSTANCE hInstance, HWND hParent);
	virtual ITEM* AddItem(const std::wstring& text, const size_t& id, HICON hIcon);

protected:
	//因菜单项数量限制(m_maxItemCount)等待添加的文件历史
	std::vector<std::wstring> m_arrFileHistoryForAdd;

	intptr_t LoadData();

	virtual void DrawItem(Gdiplus::Graphics& graphic
		, Gdiplus::Brush* pBrushItemBK
		, const ITEM* pItem
		, const RECT& rcItem
		, HDC hdc);
	virtual void OnLButtonDown(const POINT& pt);
};

