#pragma once
#include <EuroScopePlugIn.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <time.h>
#include <sstream>
#include "CallsignLookup.hpp"
#include "TGraphics.h"
#include "Rimcas.hpp"

using namespace std;
using namespace EuroScopePlugIn;

class CSMRRadar :
	public EuroScopePlugIn::CRadarScreen
{
public:
	CSMRRadar();
	virtual ~CSMRRadar();

	int DRAWING_TAG = 1211;

	const int TAG_CITEM_CALLSIGN = 1911;
	const int TAG_CITEM_FPBOX = 1912;
	const int TAG_CITEM_RWY = 1913;
	const int TAG_CITEM_GATE = 1914;

	// RIMCAS Menus & shit
	const int RIMCAS_CLOSE = EuroScopePlugIn::TAG_ITEM_FUNCTION_NO;
	const int RIMCAS_ACTIVE_AIRPORT = 7999;
	const int RIMCAS_ACTIVE_AIRPORT_FUNC = 8008;
	const int RIMCAS_DISPLAY_MENU = 8000;
	const int RIMCAS_RIMCAS_MENU = 8001;
	const int RIMCAS_QDM_TOGGLE = 8006;
	const int RIMCAS_CA_MONITOR = 9873;
	const int RIMCAS_CA_MONITOR_FUNC = 8022;
	const int RIMCAS_CA_ARRIVAL = 9874;
	const int RIMCAS_CA_ARRIVAL_FUNC = 8020;
	const int RIMCAS_CLOSED_RUNWAYS = 9875;
	const int RIMCAS_CLOSED_RUNWAYS_FUNC = 8021;
	const int RIMCAS_TAGS_MENU = 8007;
	const int RIMCAS_TAGS_2NDLINE = 8009;
	const int RIMCAS_APPWINDOW = 8010;
	const int RIMCAS_UPDATERANGE = 8011;
	const int RIMCAS_UPDATEFILTER = 8012;
	const int RIMCAS_PRIMARY = 8013;
	const int RIMCAS_TAGCOLOR = 8014;
	const int RIMCAS_TIMER = 8015;
	const int RIMCAS_TAGS_ACTYPE = 8016;
	const int RIMCAS_TAGS_SQWARNING = 8017;
	const int RIMCAS_TAGS_SPEEDGATE = 8018;

	bool BLINK = false;

	map<string, POINT> TagsOffsets;

	vector<string> Active_Arrivals;

	clock_t clock_init, clock_final;

	COLORREF SMR_TARGET_COLOR = RGB(255, 242, 73);
	COLORREF SMR_H1_COLOR = RGB(0, 255, 255);
	COLORREF SMR_H2_COLOR = RGB(0, 219, 219);
	COLORREF SMR_H3_COLOR = RGB(0, 183, 183);

	COLORREF TAG_COLOR_DEP = RGB(40, 50, 200);

	typedef struct tagPOINT2 {
		double x;
		double y;
	} POINT2;

	struct Patatoide_Points {
		map<int, POINT2> points;
		map<int, POINT2> History_one_points;
		map<int, POINT2> History_two_points;
		map<int, POINT2> History_three_points;
	};

	map<const char *, Patatoide_Points> Patatoides;

	map<string, CPosition> AirportPositions;

	map<string, bool> ClosedRunway;

	char DllPathFile[_MAX_PATH];
	string DllPath;
	CCallsignLookup * Callsigns;

	map<string, bool> ShowLists;
	map<string, RECT> ListAreas;

	bool Display2ndLine = true;

	bool appWindow = false;
	RECT appWindowArea;
	bool QDMenabled = false;

	int appWindowScale = 15;
	int appWindowFilter = 6000;

	bool showPrimaryTarget = true;
	bool TagColorIsFirst = true;
	bool DisplaySquawkWarning = true;
	bool HideAcType = false;
	bool UseSpeedForGate = false;

	const float Pi = float(atan2(0, -1));

	//---OnAsrContentLoaded--------------------------------------------

	virtual void OnAsrContentLoaded(bool Loaded);

	//---OnAsrContentToBeSaved------------------------------------------

	virtual void OnAsrContentToBeSaved(void);

	//---OnRefresh------------------------------------------------------

	virtual void OnRefresh(HDC hDC, int Phase);

	//---OnClickScreenObject-----------------------------------------

	virtual void OnClickScreenObject(int ObjectType, const char * sObjectId, POINT Pt, RECT Area, int Button);

	//---OnMoveScreenObject---------------------------------------------

	virtual void OnMoveScreenObject(int ObjectType, const char * sObjectId, POINT Pt, RECT Area, bool Released);

	//---OnCompileCommand-----------------------------------------

	virtual bool OnCompileCommand(const char * sCommandLine);

	//---RefreshAirportActivity---------------------------------------------

	virtual void RefreshAirportActivity(void);

	//---OnRadarTargetPositionUpdate---------------------------------------------

	virtual void OnRadarTargetPositionUpdate(CRadarTarget RadarTarget);

	//---Haversine---------------------------------------------
	// Heading in deg, distance in m
	inline virtual CPosition Haversine(CPosition origin, double heading, double distance) {

		float lat1 = DegToRad(float(origin.m_Latitude));
		float lon1 = DegToRad(float(origin.m_Longitude));

		float R = 6372797.560856f;

		float tbrng = DegToRad(float(heading));

		float top_lat = asin(sin(lat1)*cos(float(distance) / R) + cos(lat1)*sin(float(distance) / R)*cos(tbrng));
		float top_lon = lon1 + atan2(sin(tbrng)*sin(float(distance) / R)*cos(lat1), cos(float(distance) / R) - sin(lat1)*sin(top_lat));

		CPosition newPos;
		newPos.m_Latitude = RadToDeg(top_lat);
		newPos.m_Longitude = RadToDeg(top_lon);

		return newPos;
	}

	inline virtual float randomizeHeading(float originHead) {
		return float(fmod(originHead + float((rand() % 5) - 2), 360));
	}

	//---GetBottomLine---------------------------------------------

	virtual string GetBottomLine(const char * Callsign);

	//---LineIntersect---------------------------------------------

	inline virtual POINT getIntersectionPoint(POINT lineA, POINT lineB, POINT lineC, POINT lineD) {

		double x1 = lineA.x;
		double y1 = lineA.y;
		double x2 = lineB.x;
		double y2 = lineB.y;

		double x3 = lineC.x;
		double y3 = lineC.y;
		double x4 = lineD.x;
		double y4 = lineD.y;

		POINT p = { 0, 0 };

		double d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		if (d != 0) {
			double xi = ((x3 - x4) * (x1 * y2 - y1 * x2) - (x1 - x2) * (x3 * y4 - y3 * x4)) / d;
			double yi = ((y3 - y4) * (x1 * y2 - y1 * x2) - (y1 - y2) * (x3 * y4 - y3 * x4)) / d;

			p = { (int)xi, (int)yi };

		}
		return p;
	}

	//---startsWith-----------------------------------------

	inline virtual bool startsWith(const char *pre, const char *str)
	{
		size_t lenpre = strlen(pre), lenstr = strlen(str);
		return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
	};

	//---OnFunctionCall-------------------------------------------------

	virtual void OnFunctionCall(int FunctionId, const char * sItemString, POINT Pt, RECT Area);

	//---Radians-----------------------------------------

	inline float DegToRad(float x)
	{
		return x / 180 * Pi;
	}

	inline float RadToDeg(float x)
	{
		return x / Pi * 180;
	}

	//---OnAsrContentToBeClosed-----------------------------------------

	inline virtual void OnAsrContentToBeClosed(void)
	{
		delete this;
	};
};

