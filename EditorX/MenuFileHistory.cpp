#include "stdafx.h"
#include "MenuFileHistory.h"
#include "WinSDKUtils.h"
#include "./config/fileRecord.h"
#include "./directui/uibase.h"
#include "EditorXFrame.h"

MenuFileHistory::MenuFileHistory()
{
}

MenuFileHistory::~MenuFileHistory()
{
}

BOOL MenuFileHistory::Create(HINSTANCE hInstance, HWND hParent)
{
	if (!MenuBase::Create(hInstance, hParent)) {
		return FALSE;
	}
	delete m_pFont;
	m_pFont = new Gdiplus::Font(TEXT("Arial"), 10.0f, Gdiplus::FontStyle::FontStyleRegular);
	m_itemHeight = WinSDKUtils::GetAdaptationSizeForDPI(30);
	m_maxItemCount = 10;
	m_width = WinSDKUtils::GetAdaptationSizeForDPI(500);
	m_height = (m_marginTop << 1) + m_itemHeight * m_maxItemCount;

	LoadData();
	return TRUE;
}

MenuBase::ITEM* MenuFileHistory::AddItem(const std::wstring& text, const size_t& id, HICON hIcon)
{
	MenuBase::ITEM* pItem = MenuBase::AddItem(text, id, hIcon);
	return pItem;
}

intptr_t MenuFileHistory::LoadData()
{
	std::vector<std::wstring> arrFileHistory;
	FileRecord conf;
	intptr_t count = conf.GetRecords(arrFileHistory);
	MenuBase::ITEM *pItem;

	intptr_t i = 0;
	for (; i < count && (size_t)i < m_maxItemCount; i++) {
		pItem = AddItem(arrFileHistory[i], (size_t)i, NULL);
	}

	for (; i < count; i++) {
		m_arrFileHistoryForAdd.push_back(arrFileHistory[i]);
	}
	return m_items.size();
}

void MenuFileHistory::DrawItem(Gdiplus::Graphics& graphic
	, Gdiplus::Brush* pBrushItemBK
	, const ITEM* pItem
	, const RECT& rcItem
	, HDC hdc)
{
	//填充菜单项背景色
	graphic.FillRectangle(pBrushItemBK
		, rcItem.left - 1
		, rcItem.top - 1
		, WinSDKUtils::RectWidth(rcItem) + 1
		, WinSDKUtils::RectHeight(rcItem) + 1
	);

	//绘制菜单文本
	Gdiplus::RectF rcText;
	rcText.X = (Gdiplus::REAL)(rcItem.left);
	rcText.Y = (Gdiplus::REAL)(rcItem.top);
	rcText.Width = (Gdiplus::REAL)(rcItem.right) - rcText.X;
	rcText.Height = (Gdiplus::REAL)(rcItem.bottom) - rcText.Y;
	graphic.DrawString(pItem->text.c_str()
		, -1
		, m_pFont
		, rcText
		, m_pStringFormat
		, m_pBrushForText
	);

	//绘制边框
	if (IsHover(pItem)) {
		graphic.DrawLine(m_pPenBorder, rcItem.left, rcItem.top, rcItem.right - 1, rcItem.top);
		graphic.DrawLine(m_pPenBorder, rcItem.right - 1, rcItem.top, rcItem.right - 1, rcItem.bottom - 1);
		graphic.DrawLine(m_pPenBorder, rcItem.right - 1, rcItem.bottom - 1, rcItem.left, rcItem.bottom - 1);
		graphic.DrawLine(m_pPenBorder, rcItem.left, rcItem.bottom - 1, rcItem.left, rcItem.top);
	}
}

void MenuFileHistory::OnLButtonDown(const POINT& pt)
{
	const intptr_t index = HitTestForIndex(pt);
	if (index < 0) {
		return;
	}

	int access = _waccess(m_items[index]->text.c_str(), 0);
	if (0 == access) {
		EditorXFrame::GetInstance().OpenFile(m_items[index]->text.c_str());
	}
	else {
		FileRecord conf;
		conf.DeleteRecord(m_items[index]->text.c_str());
		EditorXFrame::GetInstance().MessageBox(TEXT("文件被拒绝访问，可能已被删除"));
		m_items.erase(m_items.begin()+ index);
		if (m_arrFileHistoryForAdd.size() > 0) {
			AddItem(m_arrFileHistoryForAdd[0], 0, NULL);
			m_arrFileHistoryForAdd.erase(m_arrFileHistoryForAdd.begin());
		}
	}
	::ShowWindow(m_hWnd, SW_HIDE);
}
