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
	Logger::info(string(__FUNCSIG__));
	RunwayAreas.clear();
	AcColor.clear();
	AcOnRunway.clear();
	TimeTable.clear();
	MonitoredRunwayArr.clear();
	MonitoredRunwayDep.clear();
	ApproachingAircrafts.clear();
}

void CRimcas::OnRefreshBegin(bool isLVP) {
	Logger::info(string(__FUNCSIG__));
	AcColor.clear();
	AcOnRunway.clear();
	TimeTable.clear();
	ApproachingAircrafts.clear();
	this->IsLVP = isLVP;
}

void CRimcas::OnRefresh(CRadarTarget Rt, CRadarScreen *instance, bool isCorrelated) {
	Logger::info(string(__FUNCSIG__));
	GetAcInRunwayArea(Rt, instance);
	GetAcInRunwayAreaSoon(Rt, instance, isCorrelated);
}

void CRimcas::AddRunwayArea(CRadarScreen *instance, string runway_name1, string runway_name2, vector<CPosition> Definition) {
	Logger::info(string(__FUNCSIG__));
	string Name = runway_name1 + " / " + runway_name2;
	
	RunwayAreaType Runway;
	Runway.Name = Name;
	Runway.Definition = Definition;

	RunwayAreas[Name] = Runway;
}

string CRimcas::GetAcInRunwayArea(CRadarTarget Ac, CRadarScreen *instance) {
	Logger::info(string(__FUNCSIG__));
	int AltitudeDif = Ac.GetPosition().GetFlightLevel() - Ac.GetPreviousPosition(Ac.GetPosition()).GetFlightLevel();
	if (!Ac.GetPosition().GetTransponderC())
		AltitudeDif = 0;

	if (Ac.GetGS() > 160 || AltitudeDif > 200)
		return string_false;

	POINT AcPosPix = instance->ConvertCoordFromPositionToPixel(Ac.GetPosition().GetPosition());

	for (std::map<string, RunwayAreaType>::iterator it = RunwayAreas.begin(); it != RunwayAreas.end(); ++it)
	{
		if (!MonitoredRunwayDep[string(it->first)])
			continue;

		vector<POINT> RunwayOnScreen;

		for (auto &Point : it->second.Definition)
		{
			RunwayOnScreen.push_back(instance->ConvertCoordFromPositionToPixel(Point));
		}
	
		if (Is_Inside(AcPosPix, RunwayOnScreen)) {
			AcOnRunway.insert(std::pair<string, string>(it->first, Ac.GetCallsign()));
			return string(it->first);
		}
	}

	return string_false;
}

string CRimcas::GetAcInRunwayAreaSoon(CRadarTarget Ac, CRadarScreen *instance, bool isCorrelated) {
	Logger::info(string(__FUNCSIG__));
	int AltitudeDif = Ac.GetPosition().GetFlightLevel() - Ac.GetPreviousPosition(Ac.GetPosition()).GetFlightLevel();
	if (!Ac.GetPosition().GetTransponderC())
		AltitudeDif = 0;
	
	// Making sure the AC is airborne and not climbing, but below transition
	if (Ac.GetGS() < 50 || 
		AltitudeDif > 50 || 
		Ac.GetPosition().GetPressureAltitude() > instance->GetPlugIn()->GetTransitionAltitude())
		return string_false;

	// If the AC is already on the runway, then there is no point in this step
	if (isAcOnRunway(Ac.GetCallsign()))
		return string_false;

	POINT AcPosPix = instance->ConvertCoordFromPositionToPixel(Ac.GetPosition().GetPosition());

	for (std::map<string, RunwayAreaType>::iterator it = RunwayAreas.begin(); it != RunwayAreas.end(); ++it)
	{
		if (!MonitoredRunwayArr[it->first])
			continue;

		// We need to know when and if the AC is going to enter the runway within 5 minutes (by steps of 10 seconds

		vector<POINT> RunwayOnScreen;

		for (auto &Point : it->second.Definition)
		{
			RunwayOnScreen.push_back(instance->ConvertCoordFromPositionToPixel(Point));
		}

		double AcSpeed = Ac.GetPosition().GetReportedGS();
		if (!Ac.GetPosition().GetTransponderC())
			AcSpeed = Ac.GetGS();

		for (int t = 5; t <= 300; t+= 5)
		{
			double distance = Ac.GetPosition().GetReportedGS()*0.514444*t;

			// We tolerate up 2 degree variations to the runway at long range (> 120 s)
			// And 3 degrees after (<= 120 t)

			bool isGoingToLand = false;
			int AngleMin = -2; 
			int AngleMax = 2;
			if (t <= 120)
			{
				AngleMin = -3;
				AngleMax = 3;
			}

			for (int a = AngleMin; a <= AngleMax;  a++)
			{
				POINT PredictedPosition = instance->ConvertCoordFromPositionToPixel(
					BetterHarversine(Ac.GetPosition().GetPosition(), fmod(Ac.GetTrackHeading() + a, 360), distance));
				isGoingToLand = Is_Inside(PredictedPosition, RunwayOnScreen);

				if (isGoingToLand)
					break;
			}

			if (isGoingToLand)
			{
				// The aircraft is going to be on the runway, we need to decide where it needs to be shown on the AIW
				bool first = true;
				vector<int> Definiton = CountdownDefinition;
				if (IsLVP)
					Definiton = CountdownDefinitionLVP;
				for (size_t k = 0; k < Definiton.size(); k++)
				{
					int Time = Definiton.at(k);

					int PreviousTime = 0;
					if (first)
					{
						PreviousTime = Time + 15;
						first = false;
					}
					else
					{
						PreviousTime = Definiton.at(k-1);
					}
					if (t < PreviousTime && t >= Time)
					{
						TimeTable[it->first][Time] = Ac.GetCallsign();
						break;
					}
				}

				// If the AC is xx seconds away from the runway, we consider him on it

				int StageTwoTrigger = 20;
				if (IsLVP)
					StageTwoTrigger = 30;

				if (t <= StageTwoTrigger)
					AcOnRunway.insert(std::pair<string, string>(it->first, Ac.GetCallsign()));

				// If the AC is 45 seconds away from the runway, we consider him approaching

				if (t > StageTwoTrigger && t <= 45)
					ApproachingAircrafts.insert(std::pair<string, string>(it->first, Ac.GetCallsign()));

				return Ac.GetCallsign();
			}
		}
	}

	return CRimcas::string_false;
}

vector<CPosition> CRimcas::GetRunwayArea(CPosition Left, CPosition Right, float hwidth) {
	Logger::info(string(__FUNCSIG__));
	vector<CPosition> out;
	
	double RunwayBearing = RadToDeg(TrueBearing(Left, Right));

	out.push_back(BetterHarversine(Left, fmod(RunwayBearing + 90, 360), hwidth)); // Bottom Left
	out.push_back(BetterHarversine(Right, fmod(RunwayBearing + 90, 360), hwidth)); // Bottom Right
	out.push_back(BetterHarversine(Right, fmod(RunwayBearing - 90, 360), hwidth)); // Top Right
	out.push_back(BetterHarversine(Left, fmod(RunwayBearing - 90, 360), hwidth)); // Top Left

	return out;
}

void CRimcas::OnRefreshEnd(CRadarScreen *instance, int threshold) {
	Logger::info(string(__FUNCSIG__));
	for (map<string, RunwayAreaType>::iterator it = RunwayAreas.begin(); it != RunwayAreas.end(); ++it)
	{

		if (!MonitoredRunwayArr[string(it->first)] && !MonitoredRunwayDep[string(it->first)])
			continue;

		bool isOnClosedRunway = false;
		if (ClosedRunway.find(it->first) != ClosedRunway.end()) {
			if (ClosedRunway[it->first])
				isOnClosedRunway = true;
		}

		bool isAnotherAcApproaching = ApproachingAircrafts.count(it->first) > 0;

		if (AcOnRunway.count(it->first) > 1 || isOnClosedRunway || isAnotherAcApproaching) {

			auto AcOnRunwayRange = AcOnRunway.equal_range(it->first);

			for (map<string, string>::iterator it2 = AcOnRunwayRange.first; it2 != AcOnRunwayRange.second; ++it2)
			{
				if (isOnClosedRunway) {
					AcColor[it2->second] = StageTwo;
				} else
				{
					if (instance->GetPlugIn()->RadarTargetSelect(it2->second.c_str()).GetGS() > threshold)
					{
						// If the aircraft is on the runway and stage two, we check if 
						// the aircraft is going towards any aircraft thats on the runway
						// if not, we don't display the warning
						bool triggerStageTwo = false;
						CRadarTarget rd1 = instance->GetPlugIn()->RadarTargetSelect(it2->second.c_str());
						CRadarTargetPositionData currentRd1 = rd1.GetPosition();
						for (map<string, string>::iterator it3 = AcOnRunwayRange.first; it3 != AcOnRunwayRange.second; ++it3)
						{
							CRadarTarget rd2 = instance->GetPlugIn()->RadarTargetSelect(it3->second.c_str());

							double currentDist = currentRd1.GetPosition().DistanceTo(rd2.GetPosition().GetPosition());
							double oldDist = rd1.GetPreviousPosition(currentRd1).GetPosition()
								.DistanceTo(rd2.GetPreviousPosition(rd2.GetPosition()).GetPosition());

							if (currentDist < oldDist)
							{
								triggerStageTwo = true;
								break;
							}
						}

						if (triggerStageTwo)
							AcColor[it2->second] = StageTwo;
					} else
					{
						AcColor[it2->second] = StageOne;
					}
				}
			}

			for (auto &ac : ApproachingAircrafts)
			{
				if (ac.first == it->first && AcOnRunway.count(it->first) > 1)
					AcColor[ac.second] = StageOne;

				if (ac.first == it->first && isOnClosedRunway)
					AcColor[ac.second] = StageTwo;
			}
		}

	}

}

bool CRimcas::isAcOnRunway(string callsign) {
	Logger::info(string(__FUNCSIG__));
	for (std::map<string, string>::iterator it = AcOnRunway.begin(); it != AcOnRunway.end(); ++it)
	{
		if (it->second == callsign)
			return true;
	}

	return false;
}

CRimcas::RimcasAlertTypes CRimcas::getAlert(string callsign)
{
	Logger::info(string(__FUNCSIG__));
	if (AcColor.find(callsign) == AcColor.end())
		return NoAlert;

	return AcColor[callsign];
}

Color CRimcas::GetAircraftColor(string AcCallsign, Color StandardColor, Color OnRunwayColor, Color RimcasStageOne, Color RimcasStageTwo) {
	Logger::info(string(__FUNCSIG__));
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
	Logger::info(string(__FUNCSIG__));
	if (isAcOnRunway(AcCallsign)) {
		return OnRunwayColor;
	}
	else {
		return StandardColor;
	}
}
