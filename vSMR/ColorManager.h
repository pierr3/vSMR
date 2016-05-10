#include "stdafx.h"
#include <GdiPlus.h>
#include <map>
#include <string>

using namespace std;

#pragma once
class CColorManager
{
protected:
	map<string, int> ColorSettings;
public:
	CColorManager()
	{
		update_brightness("label", 100);
		update_brightness("symbol", 100);
		update_brightness("afterglow", 100);
	};
	virtual ~CColorManager()
	{
		ColorSettings.clear();
	};

	void update_brightness(string id, int value)
	{
		ColorSettings[id] = min(bounds_high(), max(bounds_low(), value));
	};

	int get_brightness(string id)
	{
		if (ColorSettings.find(id) != ColorSettings.end())
			return ColorSettings[id];
		return 100;
	};

	virtual Gdiplus::Color get_corrected_color(string id, Gdiplus::Color color)
	{
		if (get_brightness(id) == 100)
			return color;

		COLORREF colorr = color.ToCOLORREF();

		int r, g, b;
		r = GetRValue(colorr);
		g = GetGValue(colorr);
		b = GetBValue(colorr);
		BYTE a = color.GetAlpha();
		

		r = min(255, int((r*get_brightness(id))/100));
		g = min(255, int((g*get_brightness(id))/100));
		b = min(255, int((b*get_brightness(id))/100));

		Gdiplus::Color out(a, r, g, b);

		return out;
	};

	static int bounds_low()
	{
		return 70;
	};

	static int bounds_high()
	{
		return 130;
	};
};

