#pragma once
#include <EuroScopePlugIn.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <string>

using namespace std;
using namespace EuroScopePlugIn;

class CRimcas {
public:
	CRimcas();
	virtual ~CRimcas();

	const float Pi = float(atan2(0, -1));

	const string string_false = "!NO";

	struct RunwayAreaType {
		CPosition topLeft, topRight, bottomLeft, bottomRight;
		bool set = false;
	};

	//---Radians-----------------------------------------

	inline float DegToRad(float x)
	{
		return x / 180 * Pi;
	}

	inline float RadToDeg(float x)
	{
		return x / Pi * 180;
	}

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
	CPosition Haversine(CPosition origin, float heading, float distance) {

		float lat1 = DegToRad(float(origin.m_Latitude));
		float lon1 = DegToRad(float(origin.m_Longitude));

		float R = 6372797.560856f;

		float tbrng = DegToRad(heading);

		float top_lat = asin(sin(lat1)*cos(distance / R) + cos(lat1)*sin(distance / R)*cos(tbrng));
		float top_lon = lon1 + atan2(sin(tbrng)*sin(distance / R)*cos(lat1), cos(distance / R) - sin(lat1)*sin(top_lat));

		CPosition newPos;
		newPos.m_Latitude = RadToDeg(top_lat);
		newPos.m_Longitude = RadToDeg(top_lon);

		return newPos;
	}

	string GetAcInRunwayArea(CRadarTarget Ac, CRadarScreen *instance);
	string GetAcInRunwayAreaSoon(CRadarTarget Ac, CRadarScreen *instance);
	void AddRunwayArea(CRadarScreen *instance, string runway_name1, string runway_name2, CPosition Left, CPosition Right, float hwidth = 92.5f, float hlenght = 250.0f);
	COLORREF GetAircraftColor(string AcCallsign, COLORREF AcColor);

	bool isAcOnRunway(CRadarTarget rt);

	RunwayAreaType GetRunwayArea(CRadarScreen *instance, CPosition Left, CPosition Right, float hwidth = 92.5f, float hlenght = 250.0f);

	void OnRefreshBegin();
	void OnRefresh(CRadarTarget Rt, CRadarScreen *instance);
	void OnRefreshEnd();
	void Reset();

	COLORREF WarningColor = RGB(160, 90, 30); //RGB(180, 100, 50)
	COLORREF AlertColor = RGB(150, 0, 0);

	map<string, RunwayAreaType> RunwayAreas;
	multimap<string, string> AcOnRunway;
	map<string, map<int, string>> TimeTable;
	map<string, bool> MonitoredRunwayDep;
	map<string, bool> MonitoredRunwayArr;
	map<string, COLORREF> AcColor;

	bool RunwayTimerShort = true;

	inline string getActiveAirport() {
		return this->ActiveAirport;
	}

	inline string setActiveAirport(string value) {
		return this->ActiveAirport = value;
	}

	inline void toggleClosedRunway(string runway) {
		if (ClosedRunway.find(runway) == ClosedRunway.end())
			ClosedRunway[runway] = true;
		else
			ClosedRunway[runway] = !ClosedRunway[runway];
	}

	inline void toggleShortTimer() {
		RunwayTimerShort = !RunwayTimerShort;
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

	string ActiveAirport = "EGKK";
	map<string, bool> ClosedRunway;
};
