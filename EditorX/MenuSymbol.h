#pragma once
#include <string>
#include "MenuBase.h"

class MenuSymbol :
	public MenuBase
{
public:
	MenuSymbol();
	virtual ~MenuSymbol();

	virtual BOOL Create(HINSTANCE hInstance, HWND hParent);
	virtual bool GetItemRect(const size_t& item, RECT *pItemRect) const;

protected:
	size_t m_maxColCount;

	void AddSymbolGroup(const std::wstring& symbols);

	virtual void DrawItem(Gdiplus::Graphics& graphic
		, Gdiplus::Brush* pBrushItemBK
		, const ITEM* pItem
		, const RECT& rcItem
		, HDC hdc);
	virtual void OnLButtonDown(const POINT& pt);
};

