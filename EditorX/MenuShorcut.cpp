#include "stdafx.h"
#include "MenuShorcut.h"
#include "WinSDKUtils.h"
#include "StringUtils.h"
#include "EditorXFrame.h"
#include "./directui/uibase.h"

//for ::ShellExecute
#pragma comment(lib,"Shell32.lib")
#include "Shellapi.h"

std::wstring MenuShorcut::m_shortcutFolderPath;
MenuShorcut::MenuShorcut():MenuBase()
{
	m_maxColCount = 5;
	m_itemHeight = 0; //在Create中被初始化
}


MenuShorcut::~MenuShorcut() {}

BOOL MenuShorcut::Create(HINSTANCE hInstance, HWND hParent)
{
	if (!MenuBase::Create(hInstance, hParent)) {
		return FALSE;
	}
	m_pStringFormat->SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
	m_itemHeight = WinSDKUtils::GetAdaptationSizeForDPI(75);
	m_width = m_itemHeight* m_maxColCount+(m_marginLeft<<1);
	m_height = WinSDKUtils::GetAdaptationSizeForDPI(300);
	m_maxItemCount = m_maxColCount* (m_height- m_marginTop)/ m_itemHeight;

	LoadData();
	return TRUE;
}

intptr_t MenuShorcut::LoadData()
{
	if (m_shortcutFolderPath.length() < 1) {
		m_shortcutFolderPath = WinSDKUtils::GetModulePath() + TEXT("\\shortcut\\");
	}

	size_t i(0);
	std::wstring filename, fileext;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = ::FindFirstFile((m_shortcutFolderPath + TEXT("*.*")).c_str(), &FindFileData);
	if (hFind) {
		do {
			if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DEVICE
				|| FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY
				|| FindFileData.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN
				|| FindFileData.dwFileAttributes&FILE_ATTRIBUTE_TEMPORARY) {
				continue;
			}

			filename = FindFileData.cFileName;
			if (filename.length() < 5) {
				continue;
			}
			fileext = StringUtils::to_lowercase(filename.substr(filename.length() - 4));
			if (fileext.compare(TEXT(".exe"))
				&& fileext.compare(TEXT(".bat"))
				&& fileext.compare(TEXT(".lnk"))
				&& fileext.compare(TEXT(".chm"))
				&& fileext.compare(TEXT(".jar"))) {
				continue;
			}

			MenuBase::ITEM *pItem = AddItem(filename.substr(0, filename.length() - 4)
				, i++
				, WinSDKUtils::GetFileIcon(m_shortcutFolderPath + filename));
			pItem->strUserData = filename;
			pItem->iconSize.cx = pItem->iconSize.cy = (LONG)WinSDKUtils::GetAdaptationSizeForDPI(32);

			if (i == m_maxItemCount) {//数量限制
				break;
			}
		} while (::FindNextFile(hFind, &FindFileData));
		::FindClose(hFind);
	}
	return static_cast<int>(m_items.size());
}

bool MenuShorcut::GetItemRect(const size_t& item, RECT *pItemRect) const
{
	if (m_items.size() - 1 < item) {
		assert(false);
		return false;
	}

	const size_t colIndex = item % m_maxColCount;
	const size_t rowIndex = item / m_maxColCount;
	pItemRect->left = (LONG)(m_marginLeft+colIndex * m_itemHeight);
	pItemRect->top = (LONG)(m_marginTop+rowIndex * m_itemHeight);
	pItemRect->right = (LONG)(pItemRect->left + m_itemHeight);
	pItemRect->bottom = (LONG)(pItemRect->top + m_itemHeight);
	return true;
}

void MenuShorcut::DrawItem(Gdiplus::Graphics& graphic
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

	//绘制菜单图标
	/*if (m_items[i].pImage) {
	graphic.DrawImage(m_items[i].pImage
	, (Gdiplus::REAL)(rcItem.left + (m_itemHeight - m_items[i].iconSize.cx) / 2)
	, (Gdiplus::REAL)(rcItem.top + WinSDKUtils::GetAdaptationSizeForDPI(5))
	, (Gdiplus::REAL)(m_items[i].iconSize.cx)
	, (Gdiplus::REAL)(m_items[i].iconSize.cy)
	);
	}*/
	if (pItem->hIcon) {
		::DrawIconEx(hdc
			, (LONG)(rcItem.left + (m_itemHeight - pItem->iconSize.cx) / 2)
			, (LONG)(rcItem.top + WinSDKUtils::GetAdaptationSizeForDPI(3))
			, pItem->hIcon
			, 0, 0, NULL, NULL, DI_NORMAL | DI_DEFAULTSIZE);
	}

	//绘制菜单文本
	Gdiplus::RectF rcText;
	rcText.X = (Gdiplus::REAL)(rcItem.left);
	rcText.Y = (Gdiplus::REAL)(rcItem.top + WinSDKUtils::GetAdaptationSizeForDPI(5) + pItem->iconSize.cy);
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

void MenuShorcut::OnLButtonDown(const POINT& pt)
{
	const ITEM* pItem = HitTest(pt);
	if (!pItem) {
		return;
	}

	const std::wstring file = m_shortcutFolderPath + pItem->strUserData;
	if ((HINSTANCE)ERROR_FILE_NOT_FOUND
		== ::ShellExecute(NULL, TEXT("open"), file.c_str(), NULL, NULL, SW_SHOW))
	{
		int err = GetLastError();
		assert(false);
		EditorXFrame::GetInstance().MessageBox(StringUtils::format(TEXT("打开【%s】失败"), file.c_str()).c_str());
	}
}
