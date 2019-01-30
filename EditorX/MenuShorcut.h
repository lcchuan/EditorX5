#pragma once
#include "MenuBase.h"
#include<string>
#include<vector>

class MenuShorcut : public MenuBase
{
public:
	MenuShorcut();
	virtual ~MenuShorcut();

	virtual BOOL Create(HINSTANCE hInstance, HWND hParent);
	virtual bool GetItemRect(const size_t& item, RECT *pItemRect) const;

protected:
	//加载快捷方式(即设置m_arrShortCut),返回快捷方式的条数
	static std::wstring m_shortcutFolderPath;

	size_t m_maxColCount; //最大列数
	//m_itemHeight作为磁贴的边长

	intptr_t LoadData();

	virtual void DrawItem(Gdiplus::Graphics& graphic
		, Gdiplus::Brush* pBrushItemBK
		, const ITEM* pItem
		, const RECT& rcItem
		, HDC hdc);
	virtual void OnLButtonDown(const POINT& pt);
};

