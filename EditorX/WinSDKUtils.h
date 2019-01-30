#pragma once
#include<string>

class WinSDKUtils
{
public:
	static std::wstring GetModulePath();

	//@return 使用完毕需调用DestroyIcon
	static HICON GetFileIcon(const std::wstring& filepath);

	//为了适配高分辨的机器,建议不要在构造函数中调用该函数，可能会导致错误结果
	static intptr_t GetAdaptationSizeForDPI(const intptr_t& n);

	static inline void FillSolidRect(const HDC& hdc, const RECT& rect, const COLORREF& color) {
		::SetBkColor(hdc, color);
		::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
	}

	/*
	 * 判断两个矩形是否相交
	 * @return 0-不相交；1-相交；-1-相等
	 */
	static inline intptr_t IsIntersectRect(const RECT& rc1, const RECT& rc2) {
		if (rc1.left == rc2.left && rc1.top == rc2.top && rc1.right == rc2.right && rc1.bottom == rc2.bottom) {
			return -1;
		}
		return (rc1.left < rc2.right && rc1.top < rc2.bottom && rc2.left < rc1.right && rc2.top < rc1.bottom);
	}
	static inline int RectWidth(const RECT& rect) { return ::abs(rect.right - rect.left); }
	static inline int RectHeight(const RECT& rect) { return ::abs(rect.bottom - rect.top); }
	static void ClientToScreen(const HWND& hwnd, RECT& rect);

protected:
	static std::wstring m_appFolderPath;

	//为了适配高分辨的机器,使用时请调用GetAdaptationSizeForDPI
	static size_t m_zoomScale; //屏幕缩放比例
};

