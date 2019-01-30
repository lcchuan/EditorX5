#include "stdafx.h"
#include "ColorHelp.h"

COLORREF ColorHelp::ChangeLuminance(const COLORREF& color, const char& plus)
{
	if (0 == plus) {
		return color;
	}
	char y, u, v;
	unsigned char r, g, b;
	ExtractRGB(color, r, g, b);
	RGBToYUV(r, g, b, y, u, v);
	if (v + plus > 127) {
		v = 127;
	}
	else if (v + plus < -128) {
		v = -128;
	}
	else {
		v += plus;
	}
	YUVToRGB(y, u, v, r, g, b);
	return RGB(r,g,b);
}

//ÑÕÉ«È¡·´
COLORREF ColorHelp::NegtiveColor(const COLORREF& color)
{
	unsigned char r, g, b;
	ExtractRGB(color, r, g, b);
	return RGB(255 - r, 255 - g, 255 - b);
}

void ColorHelp::RGBToYUV(const unsigned char& r, const unsigned char& g, const unsigned char& b
	, char& y, char& u, char& v)
{
	y = char(0.299f*r + 0.587f*g + 0.114f*b);
	u = char(-0.147f*r - 0.289f*g + 0.436f*b);
	v = char(0.615f*r - 0.515f*g - 0.100f*b);
}

void ColorHelp::YUVToRGB(const char& y, const char& u, const char& v
	, unsigned char& r, unsigned char& g, unsigned char& b)
{
	r = unsigned char(y + 1.14f*v);
	g = unsigned char(y - 0.39f*u - 0.58f*v);
	b = unsigned char(y + 2.03f*u);
}