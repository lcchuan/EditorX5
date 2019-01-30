#include "stdafx.h"
#include "MenuSymbol.h"
#include "WinSDKUtils.h"
#include "EditorXFrame.h"
#include "ScintillaEdit.h"
#include "./directui/UIFrame.h"

MenuSymbol::MenuSymbol()
{
	m_maxColCount = 15;
}


MenuSymbol::~MenuSymbol()
{
}

BOOL MenuSymbol::Create(HINSTANCE hInstance, HWND hParent)
{
	if (!MenuBase::Create(hInstance, hParent)) {
		return FALSE;
	}

	delete m_pFont;
	m_pFont = new Gdiplus::Font(TEXT("新宋体"), 12.0f, Gdiplus::FontStyle::FontStyleRegular);

	m_pStringFormat->SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	m_itemHeight = WinSDKUtils::GetAdaptationSizeForDPI(40);
	m_width = m_itemHeight* m_maxColCount+m_marginLeft+ m_marginLeft;
	//m_height = WinSDKUtils::GetAdaptationSizeForDPI(300);

	//初始化符号数据
	AddSymbolGroup(TEXT("√×⊕【】︻︼「」『』※§○●☆★♀♂"));
	AddSymbolGroup(TEXT("↑↓←→↖↗↙↘"));
	AddSymbolGroup(TEXT("≈≌≡≠≤≥≮≯±∫∮∝∞∑∏π∩∪∈∵∴㏒㏑‰"));
	AddSymbolGroup(TEXT("￥℃℉㎜㎝㏕㎞㎡㎎㎏Ω"));
	AddSymbolGroup(TEXT("ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩⅪⅫ"));

	//计算对话框高度 m_height
	size_t rowCount = m_items.size() / m_maxColCount;
	if (m_items.size() % m_maxColCount > 0) {
		rowCount += 1;
	}
	m_height = m_itemHeight * rowCount + m_marginTop + m_marginTop;
	return TRUE;
}

bool MenuSymbol::GetItemRect(const size_t& item, RECT *pItemRect) const
{
	if (m_items.size() - 1 < item) {
		assert(false);
		return false;
	}

	const size_t colIndex = item % m_maxColCount;
	const size_t rowIndex = item / m_maxColCount;
	pItemRect->left = (LONG)(m_marginLeft + colIndex * m_itemHeight);
	pItemRect->top = (LONG)(m_marginTop + rowIndex * m_itemHeight);
	pItemRect->right = (LONG)(pItemRect->left + m_itemHeight);
	pItemRect->bottom = (LONG)(pItemRect->top + m_itemHeight);
	return true;
}

void MenuSymbol::AddSymbolGroup(const std::wstring& symbols)
{
	std::wstring symbol;
	for (size_t i = 0; i < symbols.length(); i++) {
		symbol = symbols[i];
		AddItem(symbol, 0, NULL);
	}

	//行补齐
	size_t fillCount = m_items.size() % m_maxColCount;
	if (fillCount > 0) {
		fillCount = m_maxColCount - fillCount;
		for (size_t i = 0; i < fillCount; i++) {
			AddItem(TEXT(" "),0,NULL);
		}
	}
}

void MenuSymbol::DrawItem(Gdiplus::Graphics& graphic
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
	Gdiplus::RectF rcText = lcc_direct_ui::UIBase::ConvertToGDIPlusRect(rcItem);
	graphic.DrawString(pItem->text.c_str()
		, -1
		, m_pFont
		, rcText
		, m_pStringFormat
		, m_pBrushForText
	);

	//绘制边框
	graphic.DrawLine(m_pPenBorder, rcItem.left, rcItem.top, rcItem.right - 1, rcItem.top);
	graphic.DrawLine(m_pPenBorder, rcItem.right - 1, rcItem.top, rcItem.right - 1, rcItem.bottom - 1);
	graphic.DrawLine(m_pPenBorder, rcItem.right - 1, rcItem.bottom - 1, rcItem.left, rcItem.bottom - 1);
	graphic.DrawLine(m_pPenBorder, rcItem.left, rcItem.bottom - 1, rcItem.left, rcItem.top);
}

void MenuSymbol::OnLButtonDown(const POINT& pt)
{
	const ITEM* pItem = HitTest(pt);
	if (!pItem || pItem->text.compare(TEXT(" ")) == 0) {
		return;
	}

	ScintillaEdit* pEditor = EditorXFrame::GetInstance().GetActiveEditor();
	if (pEditor) {
		pEditor->ReplaceSel(pItem->text);
	}
	else {
		lcc_direct_ui::UIFrame::GetInstance().Hint(TEXT("当前无活动的编辑器,无法插入符号"));
	}

	::ShowWindow(m_hWnd, SW_HIDE);
}
