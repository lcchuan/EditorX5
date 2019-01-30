#pragma once
#include<string>

class GdiplusHelp
{
public:
	//如果先Gdiplus::GdiplusShutdown(m_gdiplusToken);再delete Gdiplus对象; 程序会崩溃。
#define DELETE_GDIPLUS_OBJ(pObj) if (GdiplusHelp::IsGdiplusValid()) delete (pObj);

	/* 可借此判断Gdiplus环境是否有效
	* ！！ 如果先Gdiplus::GdiplusShutdown(m_gdiplusToken);再delete Gdiplus对象; 程序会崩溃。
	* 所以需要先借此函数判断
	*/
	static inline bool IsGdiplusValid() { return m_gdiplusToken != 0; }
	//初始化Gdiplus环境,仅能被调用一次！！
	static void LoadGdiplus() { Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL); }
	//初始化Gdiplus环境,仅能被调用一次！！
	static void UnloadGdiplus() { Gdiplus::GdiplusShutdown(m_gdiplusToken); m_gdiplusToken = 0; }

	//生成圆角矩形
	static bool GetRoundRectPath(const INT& x, const INT& y, const INT& width, const INT& height, const INT& radius
		, Gdiplus::GraphicsPath& path);

	//填充圆角矩形的背景
	static void FillRoundRect(Gdiplus::Graphics& graphic, const Gdiplus::Brush *pBrushBK
		, const INT& x, const INT& y, const INT& width, const INT& height, const INT& radius);
	static void FillRoundRect(Gdiplus::Graphics& graphic, const Gdiplus::Brush *pBrushBK
		, const Gdiplus::RectF& rectf, const INT& radius);

	//绘制圆角矩形
	static void DrawRoundRect(Gdiplus::Graphics& graphic, const Gdiplus::Pen *pPen
		, const INT& x, const INT& y, const INT& width, const INT& height, const INT& radius
	);
	static void DrawRoundRect(Gdiplus::Graphics& graphic, const Gdiplus::Pen *pPen
		, const Gdiplus::RectF& rectf, const INT& radius
	);

private:
	static ULONG_PTR m_gdiplusToken;
	static Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
};

