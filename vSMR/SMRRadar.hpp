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
#include <GdiPlus.h>
#include "Constant.hpp"
#include "CallsignLookup.hpp"
#include "Config.hpp"
#include "TGraphics.h"
#include "Rimcas.hpp"

using namespace std;
using namespace Gdiplus;
using namespace EuroScopePlugIn;

class CSMRRadar :
	public EuroScopePlugIn::CRadarScreen
{
public:
	CSMRRadar();
	virtual ~CSMRRadar();

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

	map<int, int> appWindowIds;
	map<int, bool> appWindowDisplays;
	map<int, RECT> appWindowAreas;
	map<int, POINT> appWindowOffsets;
	map<int, int> appWindowScales;
	map<int, int> appWindowFilters;

	bool QDMenabled = false;

	bool ColorSettingsDay = true;

	bool isLVP = false;

	map<string, RECT> TimePopupAreas;

	map<int, string> TimePopupData;
	multimap<string, string> AcOnRunway;
	map<string, bool> ColorAC;

	map<string, CRimcas::RunwayAreaType> RunwayAreas;

	map<string, RECT> MenuPositions;
	map<string, bool> DisplayMenu;

	map<string, string> ManuallyCorrelated;

	CRimcas * RimcasInstance = NULL;
	CConfig * CurrentConfig = NULL;

	//----
	// Tag types
	//---

	enum TagTypes { Departure, Arrival, Airborne, Uncorrelated };

	//---ActiveAirport--------------------------------------------

	string ActiveAirport = "LFPG";

	inline string getActiveAirport() {
		return this->ActiveAirport;
	}

	inline string setActiveAirport(string value) {
		return this->ActiveAirport = value;
	}

	//---IsCorrelatedFuncs---------------------------------------------

	inline virtual bool IsCorrelated(CFlightPlan fp, CRadarTarget rt)
	{
		
		if (CurrentConfig->getActiveProfile()["filters"]["pro_mode"]["enable"].GetBool())
		{
			if (fp.IsValid())
			{
				bool isCorr = false;
				if (strcmp(fp.GetControllerAssignedData().GetSquawk(), rt.GetPosition().GetSquawk()) == 0)
				{
					isCorr = true;
				}

				if (CurrentConfig->getActiveProfile()["filters"]["pro_mode"]["accept_pilot_squawk"].GetBool())
				{
					isCorr = true;
				}


				if (isCorr)
				{
					const Value& sqs = CurrentConfig->getActiveProfile()["filters"]["pro_mode"]["do_not_autocorrelate_squawks"];
					for (SizeType i = 0; i < sqs.Size(); i++) {
						if (strcmp(rt.GetPosition().GetSquawk(), sqs[i].GetString()) == 0)
						{
							isCorr = false;
							break;
						}
					}
				}

				return isCorr;
			}

			return false;
		} else
		{
			// If the pro mode is not used, then the AC is always correlated
			return true;
		}
	};

	//---LoadCustomFont--------------------------------------------

	virtual void LoadCustomFont();

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

	//---OnAsrContentToBeClosed-----------------------------------------

	inline virtual void OnAsrContentToBeClosed(void)
	{
		delete RimcasInstance;
		delete CurrentConfig;
		delete this;
	};
};

