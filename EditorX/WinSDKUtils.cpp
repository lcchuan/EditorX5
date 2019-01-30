#include "stdafx.h"
#include "WinSDKUtils.h"

//for ::ShellExecute
#pragma comment(lib,"Shell32.lib")
#include "Shellapi.h"

std::wstring WinSDKUtils::m_appFolderPath;
size_t WinSDKUtils::m_zoomScale = 0;

std::wstring WinSDKUtils::GetModulePath()
{
	if (m_appFolderPath.length() < 1) {
		TCHAR p_path[1000];
		if (::GetModuleFileName(NULL, p_path, 1000) == 0)
		{
			assert(false);
			return m_appFolderPath;
		}
		m_appFolderPath = p_path;

		size_t index = m_appFolderPath.rfind('\\');
		m_appFolderPath = m_appFolderPath.substr(0, index);
	}
	return m_appFolderPath;
}

//@return 使用完毕需调用DestroyIcon
HICON WinSDKUtils::GetFileIcon(const std::wstring& filepath)
{
	SHFILEINFO shfileinfo;
	DWORD_PTR result = ::SHGetFileInfo(filepath.c_str(), 0, &shfileinfo, sizeof(SHFILEINFO), SHGFI_ICON);
	return result ? shfileinfo.hIcon : NULL;
}

intptr_t WinSDKUtils::GetAdaptationSizeForDPI(const intptr_t& n) {
	if (m_zoomScale == 0) {
		/* windows SDK开发时，需要先调用SetProcessDPIAware()，GetDeviceCaps才会显示正确的值
		* 详见EditorX.cpp中_tWinMain函数内的注解
		*/
		HDC hdc = ::GetDC(NULL);
		intptr_t horizontalDPI = ::GetDeviceCaps(hdc, LOGPIXELSX);
		m_zoomScale = horizontalDPI / 96;
		::ReleaseDC(NULL, hdc);
	}
	if (1 == m_zoomScale) {
		return n;
	}
	else if (0 == m_zoomScale) {
		assert(false);
		return n;
	}
	else {
		return n * m_zoomScale;
	}
}

void WinSDKUtils::ClientToScreen(const HWND& hwnd, RECT& rect)
{
	POINT pt;

	pt.x = rect.left;
	pt.y = rect.top;
	::ClientToScreen(hwnd, &pt);
	rect.left = pt.x;
	rect.top = pt.y;

	pt.x = rect.right;
	pt.y = rect.bottom;
	::ClientToScreen(hwnd, &pt);
	rect.right = pt.x;
	rect.bottom = pt.y;
}
