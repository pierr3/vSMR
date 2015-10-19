#pragma once
#include <afxwin.h>
#include <string>
#include "EuroScopePlugIn.h"
#include "stdafx.h"

using namespace std;
using namespace EuroScopePlugIn;

const int TAG_ITEM_DATALINK_STS = 444;
const int TAG_FUNC_DATALINK_MENU = 544;

const int TAG_FUNC_DATALINK_CONFIRM = 545;
const int TAG_FUNC_DATALINK_STBY = 546;
const int TAG_FUNC_DATALINK_VOICE = 547;
const int TAG_FUNC_DATALINK_RESET = 548;

const float Pi = float(atan2(0, -1));

inline static bool startsWith(const char *pre, const char *str)
{
	size_t lenpre = strlen(pre), lenstr = strlen(str);
	return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
};

inline static void replaceAll(string& source, const string& from, const string& to)
{
	string newString;
	newString.reserve(source.length());  // avoids a few memory allocations

	string::size_type lastPos = 0;
	string::size_type findPos;

	while (string::npos != (findPos = source.find(from, lastPos)))
	{
		newString.append(source, lastPos, findPos - lastPos);
		newString += to;
		lastPos = findPos + from.length();
	}

	// Care for the rest after last occurrence
	newString += source.substr(lastPos);

	source.swap(newString);
};

inline static Gdiplus::Rect CopyRect(CRect &rect)
{
	return Gdiplus::Rect(rect.left, rect.top, rect.Width(), rect.Height());
};

inline double TrueBearing(CPosition pos1, CPosition pos2)
{
	const float PI = float(atan2(0, -1));

	// returns the true bearing from pos1 to pos2
	double lat1 = pos1.m_Latitude / 180 * PI;
	double lat2 = pos2.m_Latitude / 180 * PI;
	double lon1 = pos1.m_Longitude / 180 * PI;
	double lon2 = pos2.m_Longitude / 180 * PI;
	double dir = fmod(atan2(sin(lon2 - lon1) * cos(lat2), cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(lon2 - lon1)), 2 * PI) * 180 / PI;

	return dir / 180 * PI;
};

//---Radians-----------------------------------------

inline float DegToRad(float x)
{
	return x / 180 * Pi;
};

inline float RadToDeg(float x)
{
	return x / Pi * 180;
};

const int DRAWING_TAG = 1211;
const int DRAWING_AC_SYMBOL = 1212;
const int DRAWING_BACKGROUND_CLICK = 1213;

const int TAG_CITEM_NO = 1910;
const int TAG_CITEM_CALLSIGN = 1911;
const int TAG_CITEM_FPBOX = 1912;
const int TAG_CITEM_RWY = 1913;
const int TAG_CITEM_GATE = 1914;

// RIMCAS Menus & shit
const int RIMCAS_CLOSE = EuroScopePlugIn::TAG_ITEM_FUNCTION_NO;
const int RIMCAS_ACTIVE_AIRPORT = 7999;
const int RIMCAS_ACTIVE_AIRPORT_FUNC = 8008;
const int RIMCAS_MENU = 8000;
const int RIMCAS_QDM_TOGGLE = 8006;
const int RIMCAS_QDM_SELECT_TOGGLE = 8007;
const int RIMCAS_OPEN_LIST = 9873;
const int RIMCAS_CA_MONITOR_FUNC = 8022;
const int RIMCAS_CA_ARRIVAL_FUNC = 8020;
const int RIMCAS_CLOSED_RUNWAYS_FUNC = 8021;
const int RIMCAS_TIMER = 8015;
const int RIMCAS_UPDATE_PROFILE = 8016;
const int RIMCAS_UPDATE_BRIGHNESS = 8017;
const int RIMCAS_UPDATE_FONTS = 8018;
const int RIMCAS_UPDATE_LVP = 8019;

const int RIMCAS_UPDATERANGE = 6005;
const int RIMCAS_UPDATEFILTER = 6015;
const int RIMCAS_UPDATERANGE1 = 6006;
const int RIMCAS_UPDATEFILTER1 = 6016;
const int RIMCAS_UPDATERANGE2 = 6007;
const int RIMCAS_UPDATEFILTER2 = 6017;

const int APPWINDOW_BASE = 8887;
const int APPWINDOW_ONE = 8888;
const int APPWINDOW_TWO = 8889;
