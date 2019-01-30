#include "stdafx.h"
#include "GdiplusHelp.h"

ULONG_PTR GdiplusHelp::m_gdiplusToken = 0;
Gdiplus::GdiplusStartupInput GdiplusHelp::m_gdiplusStartupInput;

bool GdiplusHelp::GetRoundRectPath(const INT& x, const INT& y, const INT& width, const INT& height, const INT& radius
	, Gdiplus::GraphicsPath& path
)
{
	const INT diameter = radius * 2;
	path.Reset();
	path.AddArc(x, y, diameter, diameter, 180, 90);//左上角圆弧
	path.AddLine(x + radius, y, x + width - radius, y); // 上边
	path.AddArc(x + width - diameter, y, diameter, diameter, 270, 90); // 右上角圆弧
	path.AddLine(x + width, y + radius, x + width, y + height - radius);// 右边
	path.AddArc(x + width - diameter, y + height - diameter, diameter, diameter, 0, 90); // 右下角圆弧
	path.AddLine(x + width - radius, y + height, x + radius, y + height); // 下边
	path.AddArc(x, y + height - diameter, diameter, diameter, 90, 90);
	path.AddLine(x, y + radius, x, y + height - radius);
	return true;
}

void GdiplusHelp::FillRoundRect(Gdiplus::Graphics& graphic, const Gdiplus::Brush *pBrushBK
	, const INT& x, const INT& y, const INT& width, const INT& height, const INT& radius)
{
	//创建圆角矩形的graphicspath
	Gdiplus::GraphicsPath path;
	GetRoundRectPath(x, y, width, height, radius, path);

	graphic.FillPath(pBrushBK, &path);
}

void GdiplusHelp::FillRoundRect(Gdiplus::Graphics& graphic, const Gdiplus::Brush *pBrushBK
	, const Gdiplus::RectF& rectf, const INT& radius)
{
	FillRoundRect(graphic, pBrushBK, (INT)rectf.X, (INT)rectf.Y, (INT)rectf.Width, (INT)rectf.Height, radius);
}

void GdiplusHelp::DrawRoundRect(Gdiplus::Graphics& graphic, const Gdiplus::Pen *pPen
	, const INT& x, const INT& y, const INT& width, const INT& height, const INT& radius
) {
	//创建圆角矩形的graphicspath
	Gdiplus::GraphicsPath path;
	GetRoundRectPath(x, y, width - 1, height - 1, radius, path);

	graphic.DrawPath(pPen, &path);
}

void GdiplusHelp::DrawRoundRect(Gdiplus::Graphics& graphic, const Gdiplus::Pen *pPen
	, const Gdiplus::RectF& rectf, const INT& radius
)
{
	DrawRoundRect(graphic, pPen, (INT)rectf.X, (INT)rectf.Y, (INT)rectf.Width, (INT)rectf.Height, radius);
}