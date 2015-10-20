#pragma once
#include <EuroScopePlugIn.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GdiPlus.h>
#include <GdiPlusColor.h>
#include "Constant.hpp"

class CSMRRadar;
using namespace std;
using namespace Gdiplus;
using namespace EuroScopePlugIn;

class CRimcas {
public:
	CRimcas();
	virtual ~CRimcas();

	const string string_false = "!NO";

	struct RunwayAreaType {
		CPosition topLeft, topRight, bottomLeft, bottomRight, threshold;
		bool isCustomRunway = false;
		vector<CPosition> CustomDefinition;
		double bearing;
		bool set = false;
	};

	int Is_Left(const POINT &p0, const POINT &p1, const POINT &point)
	{
		return ((p1.x - p0.x) * (point.y - p0.y) -
			(point.x - p0.x) * (p1.y - p0.y));
	}

	bool Is_Inside(const POINT &point, const std::vector<POINT> &points_list)
	{
		// The winding number counter.
		int winding_number = 0;

		// Loop through all edges of the polygon.
		typedef std::vector<POINT>::size_type size_type;

		size_type size = points_list.size();

		for (size_type i = 0; i < size; ++i)             // Edge from point1 to points_list[i+1]
		{
			POINT point1(points_list[i]);
			POINT point2;

			// Wrap?
			if (i == (size - 1))
			{
				point2 = points_list[0];
			}
			else
			{
				point2 = points_list[i + 1];
			}

			if (point1.y <= point.y)                                    // start y <= point.y
			{
				if (point2.y > point.y)                                 // An upward crossing
				{
					if (Is_Left(point1, point2, point) > 0)             // Point left of edge
					{
						++winding_number;                               // Have a valid up intersect
					}
				}
			}
			else
			{
				// start y > point.y (no test needed)
				if (point2.y <= point.y)                                // A downward crossing
				{
					if (Is_Left(point1, point2, point) < 0)             // Point right of edge
					{
						--winding_number;                               // Have a valid down intersect
					}
				}
			}
		}

		return (winding_number != 0);
	}

	//---Haversine---------------------------------------------
	// Heading in deg, distance in m
	const double PI = (double)M_PI;

	CPosition Haversine(CPosition origin, float heading, float distance) {

		CPosition newPos;

		double d = (distance*0.00053996) / 60 * PI / 180;
		double trk = DegToRad(heading);
		double lat0 = DegToRad(origin.m_Latitude);
		double lon0 = DegToRad(origin.m_Longitude);

		double lat = asin(sin(lat0) * cos(d) + cos(lat0) * sin(d) * cos(trk));
		double lon = cos(lat) == 0 ? lon0 : fmod(lon0 + asin(sin(trk) * sin(d) / cos(lat)) + PI, 2 * PI) - PI;

		newPos.m_Latitude = RadToDeg(lat);
		newPos.m_Longitude = RadToDeg(lon);

		return newPos;
	}

	string GetAcInRunwayArea(CRadarTarget Ac, CRadarScreen *instance);
	string GetAcInRunwayAreaSoon(CRadarTarget Ac, CRadarScreen *instance, bool isCorrelated);
	void AddRunwayArea(CRadarScreen *instance, string runway_name1, string runway_name2, CPosition Left, CPosition Right, double bearing1, double bearing2, float hwidth = 92.5f, float hlenght = 250.0f);
	Color GetAircraftColor(string AcCallsign, Color StandardColor, Color OnRunwayColor, Color RimcasStageOne, Color RimcasStageTwo);
	Color GetAircraftColor(string AcCallsign, Color StandardColor, Color OnRunwayColor);

	void AddCustomRunway(string runway_name1, string runway_name2, CPosition Left, CPosition Right, vector<CPosition> definition);

	bool isAcOnRunway(string callsign);

	RunwayAreaType GetRunwayArea(CRadarScreen *instance, CPosition Left, CPosition Right, int threshold, double bearing, float hwidth = 92.5f, float hlenght = 250.0f);

	void OnRefreshBegin();
	void OnRefresh(CRadarTarget Rt, CRadarScreen *instance, bool isCorrelated);
	void OnRefreshEnd(CRadarScreen *instance, int threshold);
	void Reset();

	COLORREF WarningColor = RGB(160, 90, 30); //RGB(180, 100, 50)
	COLORREF AlertColor = RGB(150, 0, 0);

	enum RimcasAlertTypes { NoAlert, StageOne, StageTwo };

	map<string, RunwayAreaType> RunwayAreas;
	multimap<string, string> AcOnRunway;
	map<string, map<int, string>> TimeTable;
	map<string, bool> MonitoredRunwayDep;
	map<string, bool> MonitoredRunwayArr;
	map<string, COLORREF> AcColor;

	bool RunwayTimerShort = true;

	inline void toggleClosedRunway(string runway) {
		if (ClosedRunway.find(runway) == ClosedRunway.end())
			ClosedRunway[runway] = true;
		else
			ClosedRunway[runway] = !ClosedRunway[runway];
	}

	inline void toggleShortTimer(bool setting) {
		RunwayTimerShort = setting;
	}

	inline void toggleMonitoredRunwayDep(string runway) {
		if (MonitoredRunwayDep.find(runway) == MonitoredRunwayDep.end())
			MonitoredRunwayDep[runway] = true;
		else
			MonitoredRunwayDep[runway] = !MonitoredRunwayDep[runway];
	}

	inline void toggleMonitoredRunwayArr(string runway) {
		if (MonitoredRunwayArr.find(runway) == MonitoredRunwayArr.end())
			MonitoredRunwayArr[runway] = true;
		else
			MonitoredRunwayArr[runway] = !MonitoredRunwayArr[runway];
	}

	map<string, bool> ClosedRunway;
};
