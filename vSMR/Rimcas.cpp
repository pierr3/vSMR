#include "stdafx.h"
#include "Rimcas.hpp"

CRimcas::CRimcas()
{
}

CRimcas::~CRimcas()
{
	Reset();
}

void CRimcas::Reset() {
	RunwayAreas.clear();
	AcColor.clear();
	AcOnRunway.clear();
	RunwayAreas.clear();
	TimeTable.clear();
	MonitoredRunwayArr.clear();
	MonitoredRunwayDep.clear();
}

void CRimcas::OnRefreshBegin() {
	AcColor.clear();
	AcOnRunway.clear();
	TimeTable.clear();
}

void CRimcas::OnRefresh(CRadarTarget Rt, CRadarScreen *instance) {
	GetAcInRunwayArea(Rt, instance);
	GetAcInRunwayAreaSoon(Rt, instance);
}

void CRimcas::AddRunwayArea(CRadarScreen *instance, string runway_name1, string runway_name2, CPosition Left, CPosition Right, double bearing1, double bearing2, float hwidth, float hlenght) {
	RunwayAreas[runway_name1] = GetRunwayArea(instance, Left, Right, 0, bearing1, hwidth, hlenght);
	RunwayAreas[runway_name2] = GetRunwayArea(instance, Left, Right, 1, bearing2, hwidth, hlenght);
}

string CRimcas::GetAcInRunwayArea(CRadarTarget Ac, CRadarScreen *instance) {

	POINT AcPosPix = instance->ConvertCoordFromPositionToPixel(Ac.GetPosition().GetPosition());

	for (std::map<string, RunwayAreaType>::iterator it = RunwayAreas.begin(); it != RunwayAreas.end(); ++it)
	{
		if (!MonitoredRunwayDep[string(it->first)])
			continue;

		POINT tTopLeft = instance->ConvertCoordFromPositionToPixel(it->second.topLeft);
		POINT tTopRight = instance->ConvertCoordFromPositionToPixel(it->second.topRight);
		POINT tBottomLeft = instance->ConvertCoordFromPositionToPixel(it->second.bottomLeft);
		POINT tBottomRight = instance->ConvertCoordFromPositionToPixel(it->second.bottomRight);

		vector<POINT> RwyPolygon = { tTopLeft, tTopRight, tBottomRight, tBottomLeft };

		if (Is_Inside(AcPosPix, RwyPolygon) && Ac.GetGS() < 180) {
			AcOnRunway.insert(std::pair<string, string>(it->first, Ac.GetCallsign()));
			return string(it->first);
		}
	}

	return string_false;
}

string CRimcas::GetAcInRunwayAreaSoon(CRadarTarget Ac, CRadarScreen *instance) {
	if (Ac.GetGS() < 25)
		return CRimcas::string_false;

	POINT AcPosPix = instance->ConvertCoordFromPositionToPixel(Ac.GetPosition().GetPosition());

	for (std::map<string, RunwayAreaType>::iterator it = RunwayAreas.begin(); it != RunwayAreas.end(); ++it)
	{
		if (!MonitoredRunwayArr[it->first])
			continue;

		if (RunwayTimerShort) {
			for (int i = 15; i <= 60; i = i + 15) {
				POINT tTopLeft = instance->ConvertCoordFromPositionToPixel(it->second.topLeft);
				POINT tTopRight = instance->ConvertCoordFromPositionToPixel(it->second.topRight);
				POINT tBottomLeft = instance->ConvertCoordFromPositionToPixel(it->second.bottomLeft);
				POINT tBottomRight = instance->ConvertCoordFromPositionToPixel(it->second.bottomRight);

				vector<POINT> RwyPolygon = { tTopLeft, tTopRight, tBottomRight, tBottomLeft };

				float Distance = float(Ac.GetGS())*0.514444f;
				Distance = Distance * float(i);
				CPosition TempPosition = Haversine(Ac.GetPosition().GetPosition(), float(Ac.GetTrackHeading()), Distance);
				POINT TempPoint = instance->ConvertCoordFromPositionToPixel(TempPosition);

				if (Is_Inside(TempPoint, RwyPolygon) && !Is_Inside(AcPosPix, RwyPolygon) && Ac.GetGS() < 200) {
					TimeTable[it->first][i] = string(Ac.GetCallsign());

					// If aircraft is 30 seconds from landing, then it's considered on the runway

					if (i == 30 || i == 15)
						AcOnRunway.insert(std::pair<string, string>(it->first, string(Ac.GetCallsign())));

					return string(it->first);
					break;
				}
			}
		}
		else {
			for (int i = 30; i <= 120; i = i + 30) {
				POINT tTopLeft = instance->ConvertCoordFromPositionToPixel(it->second.topLeft);
				POINT tTopRight = instance->ConvertCoordFromPositionToPixel(it->second.topRight);
				POINT tBottomLeft = instance->ConvertCoordFromPositionToPixel(it->second.bottomLeft);
				POINT tBottomRight = instance->ConvertCoordFromPositionToPixel(it->second.bottomRight);

				vector<POINT> RwyPolygon = { tTopLeft, tTopRight, tBottomRight, tBottomLeft };

				float Distance = float(Ac.GetGS())*0.514444f;
				Distance = Distance * float(i);
				CPosition TempPosition = Haversine(Ac.GetPosition().GetPosition(), float(Ac.GetTrackHeading()), Distance);
				POINT TempPoint = instance->ConvertCoordFromPositionToPixel(TempPosition);

				if (Is_Inside(TempPoint, RwyPolygon) && !Is_Inside(AcPosPix, RwyPolygon)) {
					TimeTable[it->first][i] = string(Ac.GetCallsign());

					// If aircraft is 30 seconds from landing, then it's considered on the runway

					if (i == 30)
						AcOnRunway.insert(std::pair<string, string>(it->first, string(Ac.GetCallsign())));

					return string(it->first);
					break;
				}
			}
		}


	}

	return CRimcas::string_false;
}

CRimcas::RunwayAreaType CRimcas::GetRunwayArea(CRadarScreen *instance, CPosition Left, CPosition Right, int threshold, double bearing, float hwidth, float hlenght) {

	float heading = float(Left.DirectionTo(Right));
	float rheading = float(fmod(heading + 180, 360));

	Left = Haversine(Left, rheading, 250.0f);
	Right = Haversine(Right, heading, 250.0f);
	POINT LeftPt = instance->ConvertCoordFromPositionToPixel(Left);
	POINT RightPt = instance->ConvertCoordFromPositionToPixel(Right);

	float TopLeftheading = float(fmod(heading + 90, 360));
	CPosition TopLeft = Haversine(Left, TopLeftheading, 92.5f);
	POINT TopLeftPt = instance->ConvertCoordFromPositionToPixel(TopLeft);

	float BottomRightheading = float(fmod(heading - 90, 360));
	CPosition BottomRight = Haversine(Right, BottomRightheading, 92.5f);
	POINT BottomRightPt = instance->ConvertCoordFromPositionToPixel(BottomRight);

	CPosition TopRight = Haversine(Right, TopLeftheading, 92.5f);
	POINT TopRightPt = instance->ConvertCoordFromPositionToPixel(TopRight);

	CPosition BottomLeft = Haversine(Left, BottomRightheading, 92.5f);
	POINT BottomLeftPt = instance->ConvertCoordFromPositionToPixel(BottomLeft);

	RunwayAreaType toRender;

	toRender.topLeft = TopLeft;
	toRender.topRight = TopRight;
	toRender.bottomLeft = BottomLeft;
	toRender.bottomRight = BottomRight;
	toRender.bearing = bearing;
	if (threshold == 1) {
		toRender.threshold = Left;
	}
	else {
		toRender.threshold = Right;
	}

	toRender.set = true;

	return toRender;

}

void CRimcas::OnRefreshEnd(CRadarScreen *instance, int threshold) {

	for (map<string, RunwayAreaType>::iterator it = RunwayAreas.begin(); it != RunwayAreas.end(); ++it)
	{

		if (!MonitoredRunwayArr[string(it->first)] && !MonitoredRunwayDep[string(it->first)])
			continue;

		bool isOnClosedRunway = false;
		if (ClosedRunway.find(it->first) == ClosedRunway.end()) {
		}
		else {
			if (ClosedRunway[it->first])
				isOnClosedRunway = true;
		}

		if (AcOnRunway.count(string(it->first)) > 1 || isOnClosedRunway) {

			auto AcOnRunwayRange = AcOnRunway.equal_range(it->first);

			string isAnotherAcApproaching = "no";

			for (map<string, string>::iterator it2 = AcOnRunwayRange.first; it2 != AcOnRunwayRange.second; ++it2)
			{
				if (isOnClosedRunway) {
					if (isAnotherAcApproaching != "no") {
						AcColor[it2->second] = StageTwo;
					}
					else {
						AcColor[it2->second] = StageOne;
						CRadarTarget rt = instance->GetPlugIn()->RadarTargetSelect(it2->second.c_str());

						if (rt.IsValid() && rt.GetPosition().GetReportedGS() > threshold) {
							if (isAnotherAcApproaching == "no")
								isAnotherAcApproaching = it2->second;

							AcColor[it2->second] = StageTwo;
						}
					}
				}
			}

			if (isAnotherAcApproaching != "no") {
				AcColor[isAnotherAcApproaching] = StageTwo;
			}

		}

	}

}

bool CRimcas::isAcOnRunway(string callsign) {

	for (std::map<string, string>::iterator it = AcOnRunway.begin(); it != AcOnRunway.end(); ++it)
	{
		if (it->second == callsign)
			return true;
	}

	return false;
}

Color CRimcas::GetAircraftColor(string AcCallsign, Color StandardColor, Color OnRunwayColor, Color RimcasStageOne, Color RimcasStageTwo) {
	if (AcColor.find(AcCallsign) == AcColor.end()) {
		if (isAcOnRunway(AcCallsign)) {
			return OnRunwayColor;
		}
		else {
			return StandardColor;
		}
	}
	else {
		if (AcColor[AcCallsign] == StageOne) {
			return RimcasStageOne;
		}
		else {
			return RimcasStageTwo;
		}
	}
}

Color CRimcas::GetAircraftColor(string AcCallsign, Color StandardColor, Color OnRunwayColor) {
	if (isAcOnRunway(AcCallsign)) {
		return OnRunwayColor;
	}
	else {
		return StandardColor;
	}
}