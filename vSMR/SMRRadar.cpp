#include "stdafx.h"
#include "SMRRadar.hpp"

RECT TimePopupArea = { 300, 300, 430, 363 };

map<string, RECT> TimePopupAreas;

map<int, string> TimePopupData;
multimap<string, string> AcOnRunway;
map<string, bool> ColorAC;

map<string, CRimcas::RunwayAreaType> RunwayAreas;

map<string, RECT> MenuPositions;
map<string, bool> DisplayMenu;

CRimcas * RimcasInstance = NULL;

CSMRRadar::CSMRRadar()
{
	clock_init = clock();
	srand((unsigned)time(0));

	GetModuleFileNameA((HINSTANCE)&__ImageBase, DllPathFile, sizeof(DllPathFile));
	DllPath = DllPathFile;
	DllPath.resize(DllPath.size() - strlen("vSMR.dll"));

	Callsigns = new CCallsignLookup(DllPath + "\\ICAO_Airlines.txt");

	if (RimcasInstance == NULL)
		RimcasInstance = new CRimcas();

	MenuPositions["DisplayMenu"] = { 400, 400, 550, 500 };

	appWindowArea = { 200, 200, 600, 500 };

	appWindowFilter = 6000;
	appWindowScale = 15;

	RefreshAirportActivity();
}

CSMRRadar::~CSMRRadar()
{
}

void CSMRRadar::OnAsrContentLoaded(bool Loaded)
{
	const char * p_value;

	if ((p_value = GetDataFromAsr("Airport")) != NULL)
		RimcasInstance->ActiveAirport = p_value;

	if ((p_value = GetDataFromAsr("SecondLine")) != NULL)
	{
		int temp = atoi(p_value);
		Display2ndLine = false;
		if (temp == 1)
			Display2ndLine = true;
	}

	if ((p_value = GetDataFromAsr("PrimaryTarget")) != NULL)
	{
		int temp = atoi(p_value);
		showPrimaryTarget = false;
		if (temp == 1)
			showPrimaryTarget = true;
	}

	if ((p_value = GetDataFromAsr("ShortTimer")) != NULL)
	{
		int temp = atoi(p_value);
		RimcasInstance->RunwayTimerShort = false;
		if (temp == 1)
			RimcasInstance->RunwayTimerShort = true;
	}

	if ((p_value = GetDataFromAsr("TagColor")) != NULL)
	{
		int temp = atoi(p_value);
		TagColorIsFirst = false;
		if (temp == 1)
			TagColorIsFirst = true;
		else
			TAG_COLOR_DEP = RGB(15, 105, 20);
	}

	if ((p_value = GetDataFromAsr("AppWindowTopLeftX")) != NULL)
	{
		appWindowArea.left = atoi(p_value);
	}

	if ((p_value = GetDataFromAsr("AppWindowTopLeftY")) != NULL)
	{
		appWindowArea.top = atoi(p_value);
	}

	if ((p_value = GetDataFromAsr("AppWindowBottomRightX")) != NULL)
	{
		appWindowArea.right = atoi(p_value);
	}

	if ((p_value = GetDataFromAsr("AppWindowBottomRightY")) != NULL)
	{
		appWindowArea.bottom = atoi(p_value);
	}

	if ((p_value = GetDataFromAsr("AppWindowFilter")) != NULL)
	{
		appWindowFilter = atoi(p_value);
	}

	if ((p_value = GetDataFromAsr("AppWindowScale")) != NULL)
	{
		appWindowScale = atoi(p_value);
	}

	if ((p_value = GetDataFromAsr("AppWindow")) != NULL)
	{
		int temp = atoi(p_value);
		appWindow = false;
		if (temp == 1)
			appWindow = true;
	}

	if ((p_value = GetDataFromAsr("HideACType")) != NULL)
	{
		int temp = atoi(p_value);
		HideAcType = false;
		if (temp == 1)
			HideAcType = true;
	}

	if ((p_value = GetDataFromAsr("SquawkError")) != NULL)
	{
		int temp = atoi(p_value);
		DisplaySquawkWarning = false;
		if (temp == 1)
			DisplaySquawkWarning = true;
	}

	if ((p_value = GetDataFromAsr("SpeedForGate")) != NULL)
	{
		int temp = atoi(p_value);
		UseSpeedForGate = false;
		if (temp == 1)
			UseSpeedForGate = true;
	}

	// Auto load the airport config on ASR opened.
	CSectorElement rwy;
	for (rwy = GetPlugIn()->SectorFileElementSelectFirst(SECTOR_ELEMENT_RUNWAY);
		rwy.IsValid();
		rwy = GetPlugIn()->SectorFileElementSelectNext(rwy, SECTOR_ELEMENT_RUNWAY))
	{
		if (startsWith(RimcasInstance->getActiveAirport().c_str(), rwy.GetAirportName())) {
			if (rwy.IsElementActive(true, 0) || rwy.IsElementActive(false, 0)) {
				RimcasInstance->toggleMonitoredRunwayDep(rwy.GetRunwayName(0));
				if (rwy.IsElementActive(false, 0)) {
					RimcasInstance->toggleMonitoredRunwayArr(rwy.GetRunwayName(0));
				}
			}
			if (rwy.IsElementActive(true, 1) || rwy.IsElementActive(false, 1)) {
				RimcasInstance->toggleMonitoredRunwayDep(rwy.GetRunwayName(1));
				if (rwy.IsElementActive(false, 1)) {
					RimcasInstance->toggleMonitoredRunwayArr(rwy.GetRunwayName(1));
				}
			}
		}
	}
}

void CSMRRadar::OnAsrContentToBeSaved(void)
{
	SaveDataToAsr("Airport", "Active airport for RIMCAS", RimcasInstance->ActiveAirport.c_str());

	const char * to_save = "0";
	if (Display2ndLine)
		to_save = "1";
	SaveDataToAsr("SecondLine", "Display tag 2nd line", to_save);

	string temp;

	temp = std::to_string(appWindowArea.left);
	SaveDataToAsr("AppWindowTopLeftX", "Approach window position", temp.c_str());

	temp = std::to_string(appWindowArea.top);
	SaveDataToAsr("AppWindowTopLeftY", "Approach window position", temp.c_str());

	temp = std::to_string(appWindowArea.right);
	SaveDataToAsr("AppWindowBottomRightX", "Approach window position", temp.c_str());

	temp = std::to_string(appWindowArea.bottom);
	SaveDataToAsr("AppWindowBottomRightY", "Approach window position", temp.c_str());

	temp = std::to_string(appWindowFilter);
	SaveDataToAsr("AppWindowFilter", "Approach window filter", temp.c_str());

	temp = std::to_string(appWindowScale);
	SaveDataToAsr("AppWindowScale", "Approach window zoom", temp.c_str());

	to_save = "0";
	if (appWindow)
		to_save = "1";
	SaveDataToAsr("AppWindow", "Display Approach window", to_save);

	to_save = "0";
	if (showPrimaryTarget)
		to_save = "1";
	SaveDataToAsr("PrimaryTarget", "Display primary targets", to_save);

	to_save = "1";
	if (!TagColorIsFirst)
		to_save = "2";
	SaveDataToAsr("TagColor", "Tag colors", to_save);

	to_save = "0";
	if (RimcasInstance->RunwayTimerShort)
		to_save = "1";
	SaveDataToAsr("ShortTimer", "Timer lenght", to_save);

	to_save = "0";
	if (HideAcType)
		to_save = "1";
	SaveDataToAsr("HideACType", "Hide the A/c type on short tag", to_save);
	
	to_save = "0";
	if (DisplaySquawkWarning)
		to_save = "1";
	SaveDataToAsr("SquawkError", "Hide the squawk error warning", to_save);

	to_save = "0";
	if (UseSpeedForGate)
		to_save = "1";
	SaveDataToAsr("SpeedForGate", "Use the assigned speed for the gate", to_save);

}

void CSMRRadar::OnMoveScreenObject(int ObjectType, const char * sObjectId, POINT Pt, RECT Area, bool Released) {

	if (ObjectType == 8001) {
		MenuPositions["DisplayMenu"] = Area;
	}

	if (ObjectType == 8888) {
		if (strcmp(sObjectId, "window") == 0)
			appWindowArea = Area;
		if (strcmp(sObjectId, "resize") == 0) {
			POINT TopLeft = { appWindowArea.left, appWindowArea.top };
			POINT BottomRight = { Area.right, Area.bottom };
			CRect newSize(TopLeft, BottomRight);
			newSize.NormalizeRect();

			if (newSize.Height() < 300) {
				newSize.top = appWindowArea.top;
				newSize.bottom = appWindowArea.bottom;
			}
				
			if (newSize.Width() < 400) {
				newSize.left = appWindowArea.left;
				newSize.right = appWindowArea.right;
			}

			appWindowArea = newSize;

		}
		if (strcmp(sObjectId, "topbar") == 0) {
			CRect appWindowRect(appWindowArea);
			appWindowRect.NormalizeRect();

			POINT TopLeft = { Area.left, Area.bottom + 1 };
			POINT BottomRight = { TopLeft.x + appWindowRect.Width(), TopLeft.y + appWindowRect.Height() };
			CRect newPos(TopLeft, BottomRight);
			newPos.NormalizeRect();

			appWindowArea = newPos;
		}
	}

	if (ObjectType == DRAWING_TAG || ObjectType == TAG_CITEM_CALLSIGN || ObjectType == TAG_CITEM_FPBOX || ObjectType == TAG_CITEM_RWY || ObjectType == TAG_CITEM_GATE) {
		CRadarTarget rt = GetPlugIn()->RadarTargetSelect(sObjectId);

		if (rt.IsValid()) {
			CRect Temp = Area;
			POINT TagCenterPix = Temp.CenterPoint();
			POINT AcPosPix = ConvertCoordFromPositionToPixel(GetPlugIn()->RadarTargetSelect(sObjectId).GetPosition().GetPosition());
			POINT CustomTag = { TagCenterPix.x - AcPosPix.x, TagCenterPix.y - AcPosPix.y };
			TagsOffsets[sObjectId] = CustomTag;

			GetPlugIn()->SetASELAircraft(GetPlugIn()->FlightPlanSelect(sObjectId));

			RequestRefresh();
		}
	}

	if (ObjectType == 7000) {
		TimePopupAreas[sObjectId] = Area;
		RequestRefresh();
	}

}

void CSMRRadar::OnClickScreenObject(int ObjectType, const char * sObjectId, POINT Pt, RECT Area, int Button)
{

	if (ObjectType == 8888) {
		if (strcmp(sObjectId, "close") == 0)
			appWindow = false;
		if (strcmp(sObjectId, "range") == 0) {
			GetPlugIn()->OpenPopupList(Area, "Range", 1);
			GetPlugIn()->AddPopupListElement("25", "", RIMCAS_UPDATERANGE);
			GetPlugIn()->AddPopupListElement("20", "", RIMCAS_UPDATERANGE);
			GetPlugIn()->AddPopupListElement("15", "", RIMCAS_UPDATERANGE);
			GetPlugIn()->AddPopupListElement("10", "", RIMCAS_UPDATERANGE);
			GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		}
		if (strcmp(sObjectId, "filter") == 0) {
			GetPlugIn()->OpenPopupList(Area, "Filter", 1);
			GetPlugIn()->AddPopupListElement("24000", "", RIMCAS_UPDATEFILTER);
			GetPlugIn()->AddPopupListElement("10000", "", RIMCAS_UPDATEFILTER);
			GetPlugIn()->AddPopupListElement("8000", "", RIMCAS_UPDATEFILTER);
			GetPlugIn()->AddPopupListElement("6000", "", RIMCAS_UPDATEFILTER);
			GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		}
	}

	if (ObjectType == RIMCAS_ACTIVE_AIRPORT) {
		GetPlugIn()->OpenPopupEdit(Area, RIMCAS_ACTIVE_AIRPORT_FUNC, RimcasInstance->ActiveAirport.c_str());
	}

	if (ObjectType == RIMCAS_DISPLAY_MENU) {

		Area.top = Area.top + 30;
		Area.bottom = Area.bottom + 30;

		GetPlugIn()->OpenPopupList(Area, "Display Menu", 1);
		GetPlugIn()->AddPopupListElement("QDR", "", RIMCAS_QDM_TOGGLE);
		GetPlugIn()->AddPopupListElement("Tags  >", "", RIMCAS_TAGS_MENU);
		GetPlugIn()->AddPopupListElement("Primary target", "", RIMCAS_PRIMARY, false, showPrimaryTarget);
		GetPlugIn()->AddPopupListElement("Approach Window", "", RIMCAS_APPWINDOW);
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
	}

	if (ObjectType == RIMCAS_RIMCAS_MENU) {

		Area.top = Area.top + 30;
		Area.bottom = Area.bottom + 30;

		GetPlugIn()->OpenPopupList(Area, "RIMCAS Menu", 1);
		GetPlugIn()->AddPopupListElement("CA Monitor", "", RIMCAS_CA_MONITOR);
		GetPlugIn()->AddPopupListElement("CA Arrival", "", RIMCAS_CA_ARRIVAL); 
		GetPlugIn()->AddPopupListElement("Closed Runways", "", RIMCAS_CLOSED_RUNWAYS);
		GetPlugIn()->AddPopupListElement("Short Timer", "", RIMCAS_TIMER, false, RimcasInstance->RunwayTimerShort);
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
	}

	if (ObjectType == DRAWING_TAG) {
		CFlightPlan Fp = GetPlugIn()->FlightPlanSelect(sObjectId);
		CRadarTarget rt = GetPlugIn()->RadarTargetSelect(sObjectId);
		if (rt.GetCorrelatedFlightPlan().IsValid()) {
			StartTagFunction(rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_TYPE_CALLSIGN, rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_FUNCTION_NO, Pt, Area);
		}
		else {
			GetPlugIn()->SetASELAircraft(Fp);
		}
	}

	if (ObjectType == TAG_CITEM_CALLSIGN) {
		CFlightPlan Fp = GetPlugIn()->FlightPlanSelect(sObjectId);
		CRadarTarget rt = GetPlugIn()->RadarTargetSelect(sObjectId);
		if (rt.GetCorrelatedFlightPlan().IsValid()) {
			StartTagFunction(rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_TYPE_CALLSIGN, rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_FUNCTION_NO, Pt, Area);
		}
		else {
			GetPlugIn()->SetASELAircraft(Fp);
		}

		if (Button == EuroScopePlugIn::BUTTON_LEFT || Button == EuroScopePlugIn::BUTTON_MIDDLE)
			StartTagFunction(rt.GetCallsign(), NULL, TAG_CITEM_CALLSIGN, rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_FUNCTION_HANDOFF_POPUP_MENU, Pt, Area);
		else if (Button == EuroScopePlugIn::BUTTON_RIGHT)
			StartTagFunction(rt.GetCallsign(), NULL, TAG_CITEM_CALLSIGN, rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_FUNCTION_COMMUNICATION_POPUP, Pt, Area);
	}

	if (ObjectType == TAG_CITEM_FPBOX) {
		CFlightPlan Fp = GetPlugIn()->FlightPlanSelect(sObjectId);
		CRadarTarget rt = GetPlugIn()->RadarTargetSelect(sObjectId);
		if (rt.GetCorrelatedFlightPlan().IsValid()) {
			StartTagFunction(rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_TYPE_CALLSIGN, rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_FUNCTION_NO, Pt, Area);
		}
		else {
			GetPlugIn()->SetASELAircraft(Fp);
		}

		StartTagFunction(rt.GetCallsign(), NULL, TAG_CITEM_FPBOX, rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_FUNCTION_OPEN_FP_DIALOG, Pt, Area);
	}

	if (ObjectType == TAG_CITEM_RWY) {
		CFlightPlan Fp = GetPlugIn()->FlightPlanSelect(sObjectId);
		CRadarTarget rt = GetPlugIn()->RadarTargetSelect(sObjectId);
		if (rt.GetCorrelatedFlightPlan().IsValid()) {
			StartTagFunction(rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_TYPE_CALLSIGN, rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_FUNCTION_NO, Pt, Area);
		}
		else {
			GetPlugIn()->SetASELAircraft(Fp);
		}

		StartTagFunction(rt.GetCallsign(), NULL, TAG_CITEM_RWY, rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_FUNCTION_ASSIGNED_RUNWAY, Pt, Area);
	}

	if (ObjectType == TAG_CITEM_GATE) {
		CFlightPlan Fp = GetPlugIn()->FlightPlanSelect(sObjectId);
		CRadarTarget rt = GetPlugIn()->RadarTargetSelect(sObjectId);
		if (rt.GetCorrelatedFlightPlan().IsValid()) {
			StartTagFunction(rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_TYPE_CALLSIGN, rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_FUNCTION_NO, Pt, Area);
		}
		else {
			GetPlugIn()->SetASELAircraft(Fp);
		}

		StartTagFunction(rt.GetCallsign(), NULL, TAG_CITEM_GATE, rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_FUNCTION_EDIT_SCRATCH_PAD, Pt, Area);
	}

	RequestRefresh();
};

void CSMRRadar::OnFunctionCall(int FunctionId, const char * sItemString, POINT Pt, RECT Area) {

	if (FunctionId == RIMCAS_APPWINDOW) {
		appWindow = !appWindow;
	}

	if (FunctionId == RIMCAS_ACTIVE_AIRPORT_FUNC) {
		RimcasInstance->ActiveAirport = sItemString;
	}

	if (FunctionId == RIMCAS_QDM_TOGGLE) {
		QDMenabled = !QDMenabled;
	}

	if (FunctionId == RIMCAS_UPDATEFILTER) {
		appWindowFilter = atoi(sItemString);
	}

	if (FunctionId == RIMCAS_UPDATERANGE) {
		appWindowScale = atoi(sItemString);
	}

	if (FunctionId == RIMCAS_TAGS_ACTYPE) {
		HideAcType = !HideAcType;
		ShowLists["Tags  >"] = true;

		RequestRefresh();
	}

	if (FunctionId == RIMCAS_TAGS_SQWARNING) {
		DisplaySquawkWarning = !DisplaySquawkWarning;
		ShowLists["Tags  >"] = true;

		RequestRefresh();
	}

	if (FunctionId == RIMCAS_PRIMARY) {
		showPrimaryTarget = !showPrimaryTarget;
	}

	if (FunctionId == RIMCAS_TAGS_SPEEDGATE) {
		UseSpeedForGate = !UseSpeedForGate;
		ShowLists["Tags  >"] = true;

		RequestRefresh();
	}

	if (FunctionId == RIMCAS_TIMER) {
		RimcasInstance->toggleShortTimer();
	}

	if (FunctionId == RIMCAS_CA_ARRIVAL_FUNC) {
		RimcasInstance->toggleMonitoredRunwayArr(string(sItemString));

		ShowLists["CA Arrival"] = true;

		RequestRefresh();
	}

	if (FunctionId == RIMCAS_CA_MONITOR_FUNC) {
		RimcasInstance->toggleMonitoredRunwayDep(string(sItemString));

		ShowLists["CA Monitor"] = true;

		RequestRefresh();
	}

	if (FunctionId == RIMCAS_CLOSED_RUNWAYS_FUNC) {
		RimcasInstance->toggleClosedRunway(string(sItemString));

		ShowLists["Closed Runways"] = true;

		RequestRefresh();
	}

	if (FunctionId == RIMCAS_TAGS_2NDLINE) {
		Display2ndLine = !Display2ndLine;

		ShowLists["Tags  >"] = true;

		RequestRefresh();
	}

	if (FunctionId == RIMCAS_TAGCOLOR) {
		if (strcmp(sItemString, "Tag Colour 1") == 0) {
			TagColorIsFirst = true;
			TAG_COLOR_DEP = RGB(40, 50, 200);
		}
		else {
			TagColorIsFirst = false;
			TAG_COLOR_DEP = RGB(15, 105, 20);
		}

		ShowLists["Tags  >"] = true;

		RequestRefresh();
	}

	if (FunctionId == RIMCAS_CA_ARRIVAL || FunctionId == RIMCAS_CA_MONITOR || FunctionId == RIMCAS_CLOSED_RUNWAYS || FunctionId == RIMCAS_TAGS_MENU) {

		ShowLists[string(sItemString)] = true;
		ListAreas[string(sItemString)] = Area;

		RequestRefresh();
	}
}

void CSMRRadar::RefreshAirportActivity(void) {

	//
	// Getting the depatures and arrivals airports
	//

	Active_Arrivals.clear();
	CSectorElement airport;
	for (airport = GetPlugIn()->SectorFileElementSelectFirst(SECTOR_ELEMENT_AIRPORT);
		airport.IsValid();
		airport = GetPlugIn()->SectorFileElementSelectNext(airport, SECTOR_ELEMENT_AIRPORT))
	{
		if (airport.IsElementActive(false)) {
			string s = airport.GetName();
			s = s.substr(0, 4);
			transform(s.begin(), s.end(), s.begin(), ::toupper);
			Active_Arrivals.push_back(s);
		}
	}
}

void CSMRRadar::OnRadarTargetPositionUpdate(CRadarTarget RadarTarget)
{
	if (!RadarTarget.IsValid() || !RadarTarget.GetPosition().IsValid())
		return;

	CRadarTargetPositionData RtPos = RadarTarget.GetPosition();

	Patatoides[RadarTarget.GetCallsign()].History_three_points = Patatoides[RadarTarget.GetCallsign()].History_two_points;
	Patatoides[RadarTarget.GetCallsign()].History_two_points = Patatoides[RadarTarget.GetCallsign()].History_one_points;
	Patatoides[RadarTarget.GetCallsign()].History_one_points = Patatoides[RadarTarget.GetCallsign()].points;

	Patatoides[RadarTarget.GetCallsign()].points.clear();

	CFlightPlan fp = GetPlugIn()->FlightPlanSelect(RadarTarget.GetCallsign());

	// All units in M
	float width = 34.0f;
	float cabin_width = 4.0f;
	float lenght = 38.0f;

	if (fp.IsValid()) {
		char wtc = fp.GetFlightPlanData().GetAircraftWtc();

		if (wtc == 'L') {
			width = 13.0f;
			cabin_width = 2.0f;
			lenght = 12.0f;
		}

		if (wtc == 'H') {
			width = 61.0f;
			cabin_width = 7.0f;
			lenght = 64.0f;
		}

		if (wtc == 'J') {
			width = 80.0f;
			cabin_width = 7.0f;
			lenght = 73.0f;
		}
	}

	//
	// Randomize size
	//
	width = width + float((rand() % 5) - 2);
	cabin_width = cabin_width + float((rand() % 5) - 2);
	lenght = lenght + float((rand() % 5) - 2);

	/*
	double x0 = ConvertCoordFromPositionToPixel(RtPos.GetPosition()).x, y0 = ConvertCoordFromPositionToPixel(RtPos.GetPosition()).y, rx = 20.0, ry = 50.0;  // circle params
	double a, da, x, y;

	int i = 0;
	for (a = 0.0; a<2.0*Pi;)         // full circle
	{
	x = x0 + (rx*cos(a));
	y = y0 + (ry*sin(a));
	a += (20.0 + (rand() % 41))*Pi / 180.0;

	POINT Temp = { x, y };
	CPosition TempPos = ConvertCoordFromPixelToPosition(Temp);

	Patatoides[RadarTarget.GetCallsign()].points[i] = { TempPos.m_Latitude, TempPos.m_Longitude };

	i++;
	}*/


	// Base shape
	//   / \
		// --   --
	// --   --
	//   |_|
	// Lenght of cone is 3m

	float trackHead = float(RadarTarget.GetPosition().GetReportedHeading());
	float inverseTrackHead = float(fmod(trackHead + 180.0f, 360));
	float leftTrackHead = float(fmod(trackHead - 90.0f, 360));
	float rightTrackHead = float(fmod(trackHead + 90.0f, 360));

	float HalfLenght = lenght / 2.0f;
	float HalfCabWidth = cabin_width / 2.0f;
	float HalfSpanWidth = lenght / 2.0f;

	//
	// Bottom middle point first

	CPosition bottomMiddle = Haversine(RtPos.GetPosition(), randomizeHeading(inverseTrackHead), HalfLenght);

	//
	// Bottom left point
	CPosition bottomLeft = Haversine(bottomMiddle, randomizeHeading(leftTrackHead), HalfCabWidth);

	//
	// Bottom-Middle left point
	CPosition bottomMiddleLeft = Haversine(bottomLeft, randomizeHeading(trackHead), HalfLenght / 2.0f);

	//
	// Middle left point
	CPosition MiddleLeft = Haversine(bottomMiddleLeft, randomizeHeading(trackHead), HalfLenght / 2.0f);

	//
	// Left Wing point
	CPosition leftWingPoint = Haversine(MiddleLeft, randomizeHeading(leftTrackHead), HalfSpanWidth);

	//
	// Top left point
	float opposite_lenght = HalfLenght - 5.0f;
	CPosition topLeftPoint = Haversine(MiddleLeft, randomizeHeading(trackHead), opposite_lenght);

	//
	// Top middle point

	CPosition TopMiddle = Haversine(RtPos.GetPosition(), randomizeHeading(trackHead), HalfLenght);

	//
	// Right side
	//

	//
	// Bottom right point
	CPosition bottomRight = Haversine(bottomMiddle, randomizeHeading(rightTrackHead), HalfCabWidth);

	//
	// Bottom-Middle right point
	CPosition bottomMiddleRight = Haversine(bottomRight, randomizeHeading(trackHead), HalfLenght / 2.0f);

	//
	// Middle right point
	CPosition MiddleRight = Haversine(bottomMiddleRight, randomizeHeading(trackHead), HalfLenght / 2.0f);

	//
	// Right Wing point
	CPosition rightWingPoint = Haversine(MiddleRight, randomizeHeading(rightTrackHead), HalfSpanWidth);

	//
	// Top right point
	CPosition topRightPoint = Haversine(MiddleRight, randomizeHeading(trackHead), opposite_lenght);

	// ------

	Patatoides[RadarTarget.GetCallsign()].points[0] = { bottomMiddle.m_Latitude, bottomMiddle.m_Longitude };
	Patatoides[RadarTarget.GetCallsign()].points[1] = { bottomLeft.m_Latitude, bottomLeft.m_Longitude };
	Patatoides[RadarTarget.GetCallsign()].points[2] = { bottomMiddleLeft.m_Latitude, bottomMiddleLeft.m_Longitude };
	Patatoides[RadarTarget.GetCallsign()].points[3] = { MiddleLeft.m_Latitude, MiddleLeft.m_Longitude };
	Patatoides[RadarTarget.GetCallsign()].points[4] = { leftWingPoint.m_Latitude, leftWingPoint.m_Longitude };
	Patatoides[RadarTarget.GetCallsign()].points[5] = { topLeftPoint.m_Latitude, topLeftPoint.m_Longitude };
	Patatoides[RadarTarget.GetCallsign()].points[6] = { TopMiddle.m_Latitude, TopMiddle.m_Longitude };
	Patatoides[RadarTarget.GetCallsign()].points[7] = { topRightPoint.m_Latitude, topRightPoint.m_Longitude };
	Patatoides[RadarTarget.GetCallsign()].points[8] = { rightWingPoint.m_Latitude, rightWingPoint.m_Longitude };
	Patatoides[RadarTarget.GetCallsign()].points[9] = { MiddleRight.m_Latitude, MiddleRight.m_Longitude };
	Patatoides[RadarTarget.GetCallsign()].points[10] = { bottomMiddleRight.m_Latitude, bottomMiddleRight.m_Longitude };
	Patatoides[RadarTarget.GetCallsign()].points[11] = { bottomRight.m_Latitude, bottomRight.m_Longitude };

	// 12 points total, so 11 from 0
	// ------
}

string CSMRRadar::GetBottomLine(const char * Callsign) {
	
	CFlightPlan fp = GetPlugIn()->FlightPlanSelect(Callsign);
	string to_render = "";
	if (fp.IsValid()) {
		to_render += fp.GetCallsign();
	
		string callsign_code = fp.GetCallsign();
		callsign_code = callsign_code.substr(0, 3);
		to_render += " (" + Callsigns->getCallsign(callsign_code) + ")";

		to_render += " (";
		to_render += fp.GetPilotName();
		to_render += "): ";
		to_render += fp.GetFlightPlanData().GetAircraftFPType();
		to_render += " ";
	
		if (fp.GetFlightPlanData().IsReceived()) {
			const char * assr = fp.GetControllerAssignedData().GetSquawk();
			const char * ssr = GetPlugIn()->RadarTargetSelect(fp.GetCallsign()).GetPosition().GetSquawk();
			if (strlen(assr) != 0 && !startsWith(ssr, assr)) {
				to_render += assr;
				to_render += ":";
				to_render += ssr;
			}
			else {
				to_render += "I:";
				to_render += ssr;
			}

			to_render += " ";
			to_render += fp.GetFlightPlanData().GetOrigin();
			to_render += "==>";
			to_render += fp.GetFlightPlanData().GetDestination();
			to_render += " (";
			to_render += fp.GetFlightPlanData().GetAlternate();
			to_render += ")";

			to_render += " at ";
			int rfl = fp.GetControllerAssignedData().GetFinalAltitude();
			string rfl_s = "";
			if (rfl == 0)
				rfl = fp.GetFlightPlanData().GetFinalAltitude();
			if (rfl > GetPlugIn()->GetTransitionAltitude())
				rfl_s = "FL" + std::to_string(rfl / 100);
			else
				rfl_s = std::to_string(rfl) + "ft";

			to_render += rfl_s;
			to_render += " Route: ";
			to_render += fp.GetFlightPlanData().GetRoute();
		}
	}

	return to_render;
}

bool CSMRRadar::OnCompileCommand(const char * sCommandLine)
{
	return false;
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

// Liang-Barsky function by Daniel White @ http://www.skytopia.com/project/articles/compsci/clipping.html
// This function inputs 8 numbers, and outputs 4 new numbers (plus a boolean value to say whether the clipped line is drawn at all).
//
bool LiangBarsky(RECT Area, POINT fromSrc, POINT toSrc , POINT &ClipFrom, POINT &ClipTo)         // The output values, so declare these outside.
{

	double edgeLeft, edgeRight, edgeBottom, edgeTop, x0src, y0src, x1src, y1src;

	edgeLeft = Area.left;
	edgeRight = Area.right;
	edgeBottom = Area.top;
	edgeTop = Area.bottom;

	x0src = fromSrc.x;
	y0src = fromSrc.y;
	x1src = toSrc.x;
	y1src = toSrc.y;

	double t0 = 0.0;    double t1 = 1.0;
	double xdelta = x1src - x0src;
	double ydelta = y1src - y0src;
	double p, q, r;

	for (int edge = 0; edge<4; edge++) {   // Traverse through left, right, bottom, top edges.
		if (edge == 0) { p = -xdelta;    q = -(edgeLeft - x0src); }
		if (edge == 1) { p = xdelta;     q = (edgeRight - x0src); }
		if (edge == 2) { p = -ydelta;    q = -(edgeBottom - y0src); }
		if (edge == 3) { p = ydelta;     q = (edgeTop - y0src); }
		r = q / p;
		if (p == 0 && q<0) return false;   // Don't draw line at all. (parallel line outside)

		if (p<0) {
			if (r>t1) return false;         // Don't draw line at all.
			else if (r>t0) t0 = r;            // Line is clipped!
		}
		else if (p>0) {
			if (r<t0) return false;      // Don't draw line at all.
			else if (r<t1) t1 = r;         // Line is clipped!
		}
	}

	ClipFrom.x = long(x0src + t0*xdelta);
	ClipFrom.y = long(y0src + t0*ydelta);
	ClipTo.x = long(x0src + t1*xdelta);
	ClipTo.y = long(y0src + t1*ydelta);

	return true;        // (clipped) line is drawn
}

double TrueBearing(CPosition pos1, CPosition pos2)
{
	const float PI = float(atan2(0, -1));

	// returns the true bearing from pos1 to pos2
	double lat1 = pos1.m_Latitude / 180 * PI;
	double lat2 = pos2.m_Latitude / 180 * PI;
	double lon1 = pos1.m_Longitude / 180 * PI;
	double lon2 = pos2.m_Longitude / 180 * PI;
	double dir = fmod(atan2(sin(lon2 - lon1) * cos(lat2), cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(lon2 - lon1)), 2 * PI) * 180 / PI;

	return dir / 180 * PI;
}

void CSMRRadar::OnRefresh(HDC hDC, int Phase)
{
	if (Phase != REFRESH_PHASE_BEFORE_TAGS)
		return;

	struct Utils {
		static RECT GetAreaFromText(CDC * dc, string text, POINT Pos) {
			RECT Area = { Pos.x, Pos.y, Pos.x + dc->GetTextExtent(text.c_str()).cx, Pos.y + dc->GetTextExtent(text.c_str()).cy };
			return Area;
		}
	};

	// Timer each seconds
	clock_final = clock() - clock_init;
	double delta_t = (double)clock_final / ((double)CLOCKS_PER_SEC);
	if (delta_t >= 1) {
		clock_init = clock();
		BLINK = !BLINK;
		RefreshAirportActivity();
	}

	CDC dc;
	dc.Attach(hDC);

	RECT RadarArea = GetRadarArea();
	RECT ChatArea = GetChatArea();
	RadarArea.bottom = ChatArea.top;

	AirportPositions.clear();

	CSectorElement apt;
	for (apt = GetPlugIn()->SectorFileElementSelectFirst(SECTOR_ELEMENT_AIRPORT);
		apt.IsValid();
		apt = GetPlugIn()->SectorFileElementSelectNext(apt, SECTOR_ELEMENT_AIRPORT))
	{
		CPosition Pos;
		apt.GetPosition(&Pos, 0);
		AirportPositions[string(apt.GetName())] = Pos;
	}

	RimcasInstance->RunwayAreas.clear();

	CSectorElement rwy;
	for (rwy = GetPlugIn()->SectorFileElementSelectFirst(SECTOR_ELEMENT_RUNWAY);
		rwy.IsValid();
		rwy = GetPlugIn()->SectorFileElementSelectNext(rwy, SECTOR_ELEMENT_RUNWAY))
	{
		if (startsWith(RimcasInstance->getActiveAirport().c_str(), rwy.GetAirportName())) {

			CPosition Left;
			rwy.GetPosition(&Left, 1);
			CPosition Right;
			rwy.GetPosition(&Right, 0);

			string runway_name = rwy.GetRunwayName(0);
			string runway_name2 = rwy.GetRunwayName(1);

			double bearing1 = TrueBearing(Left, Right);
			double bearing2 = TrueBearing(Right, Left);

			RimcasInstance->AddRunwayArea(this, runway_name, runway_name2, Left, Right, bearing1, bearing2);

			if (RimcasInstance->ClosedRunway.find(runway_name) != RimcasInstance->ClosedRunway.end() || RimcasInstance->ClosedRunway.find(runway_name2) != RimcasInstance->ClosedRunway.end()) {
				if (RimcasInstance->ClosedRunway[runway_name] || RimcasInstance->ClosedRunway[runway_name2]) {

					CRimcas::RunwayAreaType Area = RimcasInstance->GetRunwayArea(this, Left, Right, 0, bearing1);

					POINT TopLeftPt = ConvertCoordFromPositionToPixel(Area.topLeft);

					POINT BottomRightPt = ConvertCoordFromPositionToPixel(Area.bottomRight);

					POINT TopRightPt = ConvertCoordFromPositionToPixel(Area.topRight);

					POINT BottomLeftPt = ConvertCoordFromPositionToPixel(Area.bottomLeft);

					CPen RedPen(PS_SOLID, 2, RGB(150, 0, 0));
					CPen * oldPen = dc.SelectObject(&RedPen);

					dc.MoveTo(TopLeftPt);
					dc.LineTo(TopRightPt);

					dc.MoveTo(TopRightPt);
					dc.LineTo(BottomRightPt);

					dc.MoveTo(BottomRightPt);
					dc.LineTo(BottomLeftPt);

					dc.MoveTo(BottomLeftPt);
					dc.LineTo(TopLeftPt);

					dc.SelectObject(oldPen);
				}
			}
		}
	}

	RimcasInstance->OnRefreshBegin();

	// Drawing the symbols
	EuroScopePlugIn::CRadarTarget rt;
	for (rt = GetPlugIn()->RadarTargetSelectFirst();
		rt.IsValid();
		rt = GetPlugIn()->RadarTargetSelectNext(rt))
	{
		if (!rt.IsValid() || !rt.GetPosition().IsValid())
			continue;

		RimcasInstance->OnRefresh(rt, this);

		CRadarTargetPositionData RtPos = rt.GetPosition();

		CPen qTrailPen(PS_SOLID, 1, RGB(255, 255, 255));
		CPen* pqOrigPen = dc.SelectObject(&qTrailPen);

		if (rt.GetGS() < 50) {
			TGraphics th;

			POINT acPosPix = ConvertCoordFromPositionToPixel(RtPos.GetPosition());

			if (rt.GetGS() > 5) {
				POINT oldacPosPix;
				CRadarTargetPositionData pAcPos = rt.GetPosition();

				for (int i = 1; i <= 2; i++) {
					oldacPosPix = ConvertCoordFromPositionToPixel(pAcPos.GetPosition());
					pAcPos = rt.GetPreviousPosition(pAcPos);
					acPosPix = ConvertCoordFromPositionToPixel(pAcPos.GetPosition());

					int middlex = (oldacPosPix.x);
					int middley = (oldacPosPix.y);
					POINT middlePos; middlePos.x = middlex; middlePos.y = middley;

					if (i == 1 && !Patatoides[rt.GetCallsign()].History_one_points.empty() && showPrimaryTarget) {
						CBrush* pOldBrush;
						CBrush brush;
						brush.CreateSolidBrush(SMR_H1_COLOR);
						pOldBrush = dc.SelectObject(&brush);
						CPen pen(PS_SOLID, 1, SMR_H1_COLOR);
						CPen* pOldPen = dc.SelectObject(&pen);
						CPoint lpPoints[12];
						for (unsigned int i = 0; i < Patatoides[rt.GetCallsign()].History_one_points.size(); i++)
						{
							CPosition pos;
							pos.m_Latitude = Patatoides[rt.GetCallsign()].History_one_points[i].x;
							pos.m_Longitude = Patatoides[rt.GetCallsign()].History_one_points[i].y;

							lpPoints[i] = { ConvertCoordFromPositionToPixel(pos).x, ConvertCoordFromPositionToPixel(pos).y };
						}
						dc.Polygon(lpPoints, Patatoides[rt.GetCallsign()].History_one_points.size());
						dc.SelectObject(&pOldBrush);
						dc.SelectObject(&pen);
					}

					if (i != 2) {
						if (!Patatoides[rt.GetCallsign()].History_two_points.empty() && showPrimaryTarget) {
							CBrush brush;
							brush.CreateSolidBrush(SMR_H2_COLOR);
							CBrush* pOldBrush;
							pOldBrush = dc.SelectObject(&brush);
							CPen pen(PS_SOLID, 1, SMR_H2_COLOR);
							CPen* pOldPen = dc.SelectObject(&pen);
							CPoint lpPoints[12];
							for (unsigned int i = 0; i < Patatoides[rt.GetCallsign()].History_two_points.size(); i++)
							{
								CPosition pos;
								pos.m_Latitude = Patatoides[rt.GetCallsign()].History_two_points[i].x;
								pos.m_Longitude = Patatoides[rt.GetCallsign()].History_two_points[i].y;

								lpPoints[i] = { ConvertCoordFromPositionToPixel(pos).x, ConvertCoordFromPositionToPixel(pos).y };
							}
							dc.Polygon(lpPoints, Patatoides[rt.GetCallsign()].History_two_points.size());
							dc.SelectObject(&pOldBrush);
							dc.SelectObject(&pen);
						}
					}

					if (i == 2 && !Patatoides[rt.GetCallsign()].History_three_points.empty() && showPrimaryTarget) {
						CBrush brush;
						brush.CreateSolidBrush(SMR_H3_COLOR);
						CBrush* pOldBrush;
						pOldBrush = dc.SelectObject(&brush);
						CPen pen(PS_SOLID, 1, SMR_H3_COLOR);
						CPen* pOldPen = dc.SelectObject(&pen);
						CPoint lpPoints[12];
						for (unsigned int i = 0; i < Patatoides[rt.GetCallsign()].History_three_points.size(); i++)
						{
							CPosition pos;
							pos.m_Latitude = Patatoides[rt.GetCallsign()].History_three_points[i].x;
							pos.m_Longitude = Patatoides[rt.GetCallsign()].History_three_points[i].y;

							lpPoints[i] = { ConvertCoordFromPositionToPixel(pos).x, ConvertCoordFromPositionToPixel(pos).y };
						}
						dc.Polygon(lpPoints, Patatoides[rt.GetCallsign()].History_three_points.size());
						dc.SelectObject(&pOldBrush);
						dc.SelectObject(&pen);
					}

					CPen qTrailPen(PS_SOLID, 1, RGB(255, 255, 255));
					CPen* pqOrigPen = dc.SelectObject(&qTrailPen);

					dc.Rectangle(middlePos.x - 1, middlePos.y - 1, middlePos.x + 1, middlePos.y + 1);

					if (i != 2) {
						dc.Rectangle(acPosPix.x - 1, acPosPix.y - 1, acPosPix.x + 1, acPosPix.y + 1);
					}
					dc.SelectObject(pqOrigPen);
				}

			}

			if (showPrimaryTarget) {
				CBrush brush;
				brush.CreateSolidBrush(SMR_TARGET_COLOR);
				CBrush* pOldBrush;
				pOldBrush = dc.SelectObject(&brush);
				CPen pen(PS_SOLID, 1, SMR_TARGET_COLOR);
				CPen* pOldPen = dc.SelectObject(&pen);
				CPoint lpPoints[12];
				for (unsigned int i = 0; i < Patatoides[rt.GetCallsign()].points.size(); i++)
				{
					CPosition pos;
					pos.m_Latitude = Patatoides[rt.GetCallsign()].points[i].x;
					pos.m_Longitude = Patatoides[rt.GetCallsign()].points[i].y;

					lpPoints[i] = { ConvertCoordFromPositionToPixel(pos).x, ConvertCoordFromPositionToPixel(pos).y };
				}
				dc.Polygon(lpPoints, Patatoides[rt.GetCallsign()].points.size());
				dc.SelectObject(&pOldBrush);
				dc.SelectObject(&pen);
			}
			acPosPix = ConvertCoordFromPositionToPixel(RtPos.GetPosition());

			CPen qTrailPen(PS_SOLID, 1, RGB(255, 255, 255));
			CPen* pqOrigPen = dc.SelectObject(&qTrailPen);

			if (RtPos.GetTransponderC()) {
				dc.MoveTo(acPosPix.x, acPosPix.y - 6);
				dc.LineTo(acPosPix.x - 6, acPosPix.y);
				dc.MoveTo(acPosPix.x - 6, acPosPix.y);
				dc.LineTo(acPosPix.x, acPosPix.y + 6);
				dc.MoveTo(acPosPix.x, acPosPix.y + 6);
				dc.LineTo(acPosPix.x + 6, acPosPix.y);
				dc.MoveTo(acPosPix.x + 6, acPosPix.y);
				dc.LineTo(acPosPix.x, acPosPix.y - 6);
			}
			else {
				th.DrawEllipse(dc, acPosPix.x - 4, acPosPix.y - 4, acPosPix.x + 4, acPosPix.y + 4, RGB(255, 255, 255));
			}

			AddScreenObject(DRAWING_AC_SYMBOL, rt.GetCallsign(), { acPosPix.x - 4, acPosPix.y - 4, acPosPix.x + 4, acPosPix.y + 4 }, false, GetBottomLine(rt.GetCallsign()).c_str());

			dc.SelectObject(pqOrigPen);
			
		}
		else if (rt.GetGS() >= 50 && rt.GetGS() < 180 && rt.GetPosition().GetPressureAltitude() < (GetPlugIn()->GetTransitionAltitude()-2000)) {
			TGraphics th;

			POINT acPosPix = ConvertCoordFromPositionToPixel(RtPos.GetPosition());
			POINT oldacPosPix;
			CRadarTargetPositionData pAcPos = RtPos;

			for (int i = 1; i <= 2; i++) {
				oldacPosPix = ConvertCoordFromPositionToPixel(pAcPos.GetPosition());
				pAcPos = rt.GetPreviousPosition(pAcPos);
				acPosPix = ConvertCoordFromPositionToPixel(pAcPos.GetPosition());

				int middlex = (acPosPix.x + oldacPosPix.x) / 2;
				int middley = (acPosPix.y + oldacPosPix.y) / 2;
				POINT middlePos; middlePos.x = middlex; middlePos.y = middley;

				if (i == 1 && !Patatoides[rt.GetCallsign()].History_one_points.empty() && showPrimaryTarget) {
					CBrush brush;
					brush.CreateSolidBrush(SMR_H1_COLOR);
					CBrush* pOldBrush;
					pOldBrush = dc.SelectObject(&brush);
					CPen pen(PS_SOLID, 1, SMR_H1_COLOR);
					CPen* pOldPen = dc.SelectObject(&pen);
					CPoint lpPoints[12];
					for (unsigned int i = 0; i < Patatoides[rt.GetCallsign()].History_one_points.size(); i++)
					{
						CPosition pos;
						pos.m_Latitude = Patatoides[rt.GetCallsign()].History_one_points[i].x;
						pos.m_Longitude = Patatoides[rt.GetCallsign()].History_one_points[i].y;

						lpPoints[i] = { ConvertCoordFromPositionToPixel(pos).x, ConvertCoordFromPositionToPixel(pos).y };
					}
					dc.Polygon(lpPoints, Patatoides[rt.GetCallsign()].History_one_points.size());
					dc.SelectObject(&pOldBrush);
					dc.SelectObject(&pen);
				}

				if (i != 2) {
					if (!Patatoides[rt.GetCallsign()].History_two_points.empty() && showPrimaryTarget) {
						CBrush brush;
						brush.CreateSolidBrush(SMR_H2_COLOR);
						CBrush* pOldBrush;
						pOldBrush = dc.SelectObject(&brush);
						CPen pen(PS_SOLID, 1, SMR_H2_COLOR);
						CPen* pOldPen = dc.SelectObject(&pen);
						CPoint lpPoints[12];
						for (unsigned int i = 0; i < Patatoides[rt.GetCallsign()].History_two_points.size(); i++)
						{
							CPosition pos;
							pos.m_Latitude = Patatoides[rt.GetCallsign()].History_two_points[i].x;
							pos.m_Longitude = Patatoides[rt.GetCallsign()].History_two_points[i].y;

							lpPoints[i] = { ConvertCoordFromPositionToPixel(pos).x, ConvertCoordFromPositionToPixel(pos).y };
						}
						dc.Polygon(lpPoints, Patatoides[rt.GetCallsign()].History_two_points.size());
						dc.SelectObject(&pOldBrush);
						dc.SelectObject(&pen);
					}
				}

				if (i == 2 && !Patatoides[rt.GetCallsign()].History_three_points.empty() && showPrimaryTarget) {
					CBrush brush;
					brush.CreateSolidBrush(SMR_H3_COLOR);
					CBrush* pOldBrush;
					pOldBrush = dc.SelectObject(&brush);
					CPen pen(PS_SOLID, 1, SMR_H3_COLOR);
					CPen* pOldPen = dc.SelectObject(&pen);
					CPoint lpPoints[12];
					for (unsigned int i = 0; i < Patatoides[rt.GetCallsign()].History_three_points.size(); i++)
					{
						CPosition pos;
						pos.m_Latitude = Patatoides[rt.GetCallsign()].History_three_points[i].x;
						pos.m_Longitude = Patatoides[rt.GetCallsign()].History_three_points[i].y;

						lpPoints[i] = { ConvertCoordFromPositionToPixel(pos).x, ConvertCoordFromPositionToPixel(pos).y };
					}
					dc.Polygon(lpPoints, Patatoides[rt.GetCallsign()].History_three_points.size());
					dc.SelectObject(&pOldBrush);
					dc.SelectObject(&pen);
				}
			}


			acPosPix = ConvertCoordFromPositionToPixel(RtPos.GetPosition());

			if (showPrimaryTarget) {
				CBrush brush;
				brush.CreateSolidBrush(SMR_TARGET_COLOR);
				CBrush* pOldBrush;
				pOldBrush = dc.SelectObject(&brush);
				CPen pen(PS_SOLID, 1, SMR_TARGET_COLOR);
				CPen* pOldPen = dc.SelectObject(&pen);
				CPoint lpPoints[12];
				for (unsigned int i = 0; i < Patatoides[rt.GetCallsign()].points.size(); i++)
				{
					CPosition pos;
					pos.m_Latitude = Patatoides[rt.GetCallsign()].points[i].x;
					pos.m_Longitude = Patatoides[rt.GetCallsign()].points[i].y;

					lpPoints[i] = { ConvertCoordFromPositionToPixel(pos).x, ConvertCoordFromPositionToPixel(pos).y };
				}
				dc.Polygon(lpPoints, Patatoides[rt.GetCallsign()].points.size());
				dc.SelectObject(&pOldBrush);
				dc.SelectObject(&pen);
			}
			CPen qTrailPen(PS_SOLID, 1, RGB(255, 255, 255));
			CPen* pqOrigPen = dc.SelectObject(&qTrailPen);

			if (RtPos.GetTransponderC()) {
				dc.MoveTo(acPosPix.x, acPosPix.y - 6);
				dc.LineTo(acPosPix.x - 6, acPosPix.y);
				dc.MoveTo(acPosPix.x - 6, acPosPix.y);
				dc.LineTo(acPosPix.x, acPosPix.y + 6);
				dc.MoveTo(acPosPix.x, acPosPix.y + 6);
				dc.LineTo(acPosPix.x + 6, acPosPix.y);
				dc.MoveTo(acPosPix.x + 6, acPosPix.y);
				dc.LineTo(acPosPix.x, acPosPix.y - 6);
			}
			else {
				th.DrawEllipse(dc, acPosPix.x - 4, acPosPix.y - 4, acPosPix.x + 4, acPosPix.y + 4, RGB(255, 255, 255));
			}

			AddScreenObject(DRAWING_AC_SYMBOL, rt.GetCallsign(), { acPosPix.x - 4, acPosPix.y - 4, acPosPix.x + 4, acPosPix.y + 4 }, false, GetBottomLine(rt.GetCallsign()).c_str());

			dc.SelectObject(pqOrigPen);
		}
	}

	TimePopupData.clear();
	AcOnRunway.clear();
	ColorAC.clear();

	RimcasInstance->OnRefreshEnd();

	// Drawing the Tags
	for (rt = GetPlugIn()->RadarTargetSelectFirst();
		rt.IsValid();
		rt = GetPlugIn()->RadarTargetSelectNext(rt))
	{
		if (!rt.IsValid())
			continue;

		CRadarTargetPositionData RtPos = rt.GetPosition();
		POINT acPosPix = ConvertCoordFromPositionToPixel(RtPos.GetPosition());
		CFlightPlan fp = GetPlugIn()->FlightPlanSelect(rt.GetCallsign());
		POINT TagCenter;

		if (rt.GetGS() < 50) {

			//----
			// 1: Departure
			// 2: Arrival
			// 3: VFR (disabled)
			//---
			int TAG_TYPE = 1;

			if (fp.IsValid() && std::find(Active_Arrivals.begin(), Active_Arrivals.end(), fp.GetFlightPlanData().GetDestination()) != Active_Arrivals.end()) {
				TAG_TYPE = 2;
			}

			map<string, POINT>::iterator it = TagsOffsets.find(rt.GetCallsign());
			if (it != TagsOffsets.end()) {
				TagCenter = { acPosPix.x + it->second.x, acPosPix.y + it->second.y };
			}
			else {
				TagCenter = { acPosPix.x + 35, acPosPix.y - 40 };
			}

			// ----- Callsign -------
			string callsign = rt.GetCallsign();

			if (fp.IsValid()) {
				if (fp.GetControllerAssignedData().GetCommunicationType() == 't' || fp.GetControllerAssignedData().GetCommunicationType() == 'T' || fp.GetControllerAssignedData().GetCommunicationType() == 'r' || fp.GetControllerAssignedData().GetCommunicationType() == 'R') {
					callsign.append("/");
					callsign += fp.GetControllerAssignedData().GetCommunicationType();
				}
				else if (fp.GetFlightPlanData().GetCommunicationType() == 't' || fp.GetFlightPlanData().GetCommunicationType() == 'r' || fp.GetFlightPlanData().GetCommunicationType() == 'T' || fp.GetFlightPlanData().GetCommunicationType() == 'R') {
					callsign.append("/");
					callsign += fp.GetFlightPlanData().GetCommunicationType();
				}

				switch (fp.GetState()) {

				case FLIGHT_PLAN_STATE_TRANSFER_TO_ME_INITIATED:
					callsign = ">>" + callsign;
					break;

				case FLIGHT_PLAN_STATE_TRANSFER_FROM_ME_INITIATED:
					callsign = callsign + ">>";
					break;

				}

			}

			bool hadDisplayLineChanged = false;
			if (Display2ndLine && HideAcType && TAG_TYPE != 2) {
				Display2ndLine = false;
				hadDisplayLineChanged = true;
			}
				
			string line1 = callsign;
			string line2 = "";

			string error = "";
			const char * assr = fp.GetControllerAssignedData().GetSquawk();
			const char * ssr = rt.GetPosition().GetSquawk();
			bool has_squawk_error = false;
			if (strlen(assr) != 0 && !startsWith(ssr, assr)) {
				has_squawk_error = true;
				error = "A";
				error.append(assr);
			}

			string actype = fp.GetFlightPlanData().GetAircraftFPType();
			if (actype.size() > 4)
				actype = actype.substr(0, 4);

			if (has_squawk_error && DisplaySquawkWarning)
				actype = error;

			string speed = std::to_string(rt.GetGS());
			for (size_t i = 0; i < 3 - speed.length(); i++)
				speed = "0" + speed;
			speed = speed.substr(0, 3);

			string rwy = fp.GetFlightPlanData().GetDepartureRwy();

			string gate;
			if (UseSpeedForGate) {
				gate = std::to_string(fp.GetControllerAssignedData().GetAssignedSpeed());
			}
			else {
				gate = fp.GetControllerAssignedData().GetScratchPadString();
			}

			gate = gate.substr(0, 4);

			if (gate.size() == 0 || gate == "0") {
				gate = "NOGT";
			}

			if (!Display2ndLine && !HideAcType) {
				line1 += " ";
				line1 += actype;
			}
			else {
				if (TAG_TYPE == 1) {
					line2 = actype;

					if (rt.GetGS() >= 25 || RimcasInstance->isAcOnRunway(rt)) {
						line2 += " ";
						line2 += speed;
					}
					else {
						line2 += " ";
						line2 += rwy;
					}
				}
				if (TAG_TYPE == 2) {
					if (rt.GetGS() >= 25 || RimcasInstance->isAcOnRunway(rt)) {
						line2 = speed;
					}
					else {
						line2 = gate;
					}

					line2 += " ";
					line2 += actype;
				}
			}

			// ----- Draw -------
			
			string longuest = line1;
			if (Display2ndLine && line2.size() >= line1.size())
				longuest = line2;

			int TagWidth = dc.GetTextExtent(longuest.c_str()).cx;
			if (TagWidth % 2 != 0)
				TagWidth += 1;
			int TagHeight = dc.GetTextExtent(line1.c_str()).cy;

			if (Display2ndLine)
				TagHeight += dc.GetTextExtent(line2.c_str()).cy;

			POINT TopLeft = { long(TagCenter.x - float(TagWidth / 2.0f)), long(TagCenter.y - float(TagHeight / 2.0f)) };
			POINT BottomRight = { TopLeft.x + TagWidth, TopLeft.y + TagHeight };
			CRect TagArea(TopLeft, BottomRight);
			TagArea.NormalizeRect();

			CPen qLinePen(PS_SOLID, 1, RGB(255, 255, 255));
			CPen* pqOrigPen = dc.SelectObject(&qLinePen);
			dc.MoveTo(acPosPix.x, acPosPix.y);
			dc.LineTo(TagArea.CenterPoint().x, TagArea.CenterPoint().y);

			COLORREF TagColor = TAG_COLOR_DEP;
			if (TAG_TYPE == 2)
				TagColor = RGB(170, 50, 50);

			TagColor = RimcasInstance->GetAircraftColor(rt.GetCallsign(), TagColor);

			dc.FillSolidRect(TagArea, TagColor);
			dc.SelectObject(pqOrigPen);
			AddScreenObject(DRAWING_TAG, fp.GetCallsign(), TagArea, true, GetBottomLine(fp.GetCallsign()).c_str());

			dc.SetTextColor(RGB(255, 255, 255));
			int oldTextAlign = dc.SetTextAlign(TA_LEFT);

			if (fp.IsValid()) {

				int leftOffset = 0;

				// Drawing callsign
				dc.TextOutA(TagArea.left, TagArea.top, callsign.c_str());
				AddScreenObject(TAG_CITEM_CALLSIGN, rt.GetCallsign(), Utils::GetAreaFromText(&dc, callsign, { TagArea.left, TagArea.top }), true, GetBottomLine(fp.GetCallsign()).c_str());

				// Second line
				if (!Display2ndLine) {
					if (!HideAcType) {
						string t = callsign + " ";
						leftOffset += dc.GetTextExtent(t.c_str()).cx;
						COLORREF oldColor = dc.SetTextColor(RGB(255, 255, 255));
						if (has_squawk_error && DisplaySquawkWarning)
							oldColor = dc.SetTextColor(RGB(255, 255, 0));
						dc.TextOutA(TagArea.left + leftOffset, TagArea.top, actype.c_str());
						dc.SetTextColor(oldColor);
						AddScreenObject(TAG_CITEM_FPBOX, rt.GetCallsign(), Utils::GetAreaFromText(&dc, actype, { TagArea.left + leftOffset, TagArea.top }), true, GetBottomLine(fp.GetCallsign()).c_str());
					}
				}
				else {
					if (TAG_TYPE == 1) {
						if (!HideAcType) {
							int topOffset = dc.GetTextExtent("Z").cy;
							COLORREF oldColor = dc.SetTextColor(RGB(255, 255, 255));
							if (has_squawk_error && DisplaySquawkWarning)
								oldColor = dc.SetTextColor(RGB(255, 255, 0));
							dc.TextOutA(TagArea.left, TagArea.top + topOffset, actype.c_str());
							AddScreenObject(TAG_CITEM_FPBOX, rt.GetCallsign(), Utils::GetAreaFromText(&dc, actype, { TagArea.left, TagArea.top + topOffset }), true, GetBottomLine(fp.GetCallsign()).c_str());
							dc.SetTextColor(oldColor);

							string t = actype + " ";
							leftOffset += dc.GetTextExtent(t.c_str()).cx;
							dc.TextOutA(TagArea.left + leftOffset, TagArea.top + topOffset, rwy.c_str());
							AddScreenObject(TAG_CITEM_RWY, rt.GetCallsign(), Utils::GetAreaFromText(&dc, rwy, { TagArea.left + leftOffset, TagArea.top + topOffset }), true, GetBottomLine(fp.GetCallsign()).c_str());
						}
					}

					if (TAG_TYPE == 2) {
						int topOffset = dc.GetTextExtent("Z").cy;
						dc.TextOutA(TagArea.left, TagArea.top + topOffset, gate.c_str());
						AddScreenObject(TAG_CITEM_GATE, rt.GetCallsign(), Utils::GetAreaFromText(&dc, gate, { TagArea.left, TagArea.top + topOffset }), true, GetBottomLine(fp.GetCallsign()).c_str());
						
						if (!HideAcType) {
							string t = gate + " ";
							leftOffset += dc.GetTextExtent(t.c_str()).cx;
							COLORREF oldColor = dc.SetTextColor(RGB(255, 255, 255));
							if (has_squawk_error && DisplaySquawkWarning)
								oldColor = dc.SetTextColor(RGB(255, 255, 0));
							dc.TextOutA(TagArea.left + leftOffset, TagArea.top + topOffset, actype.c_str());
							dc.SetTextColor(oldColor);
							AddScreenObject(TAG_CITEM_FPBOX, rt.GetCallsign(), Utils::GetAreaFromText(&dc, actype, { TagArea.left + leftOffset, TagArea.top + topOffset }), true, GetBottomLine(fp.GetCallsign()).c_str());
						}
						
					}
				}

			}
			dc.SetTextAlign(oldTextAlign);
			if (hadDisplayLineChanged)
				Display2ndLine = true;
		}

		if (rt.GetGS() >= 50 && rt.GetGS() < 180 && rt.GetPosition().GetPressureAltitude() < 2000) {
			map<string, POINT>::iterator it = TagsOffsets.find(rt.GetCallsign());
			if (it != TagsOffsets.end()) {
				TagCenter = { acPosPix.x + it->second.x, acPosPix.y + it->second.y };
			}
			else {
				TagCenter = { acPosPix.x + 35, acPosPix.y - 40 };
			}

			RECT Temp = { TagCenter.x - 40, TagCenter.y - 20, TagCenter.x + 29, TagCenter.y + 9 };
			CRect TagArea(Temp);
			TagArea.NormalizeRect();

			CPen qLinePen(PS_SOLID, 1, RGB(255, 255, 255));
			CPen* pqOrigPen = dc.SelectObject(&qLinePen);

			dc.MoveTo(acPosPix.x, acPosPix.y);
			if (TagArea.CenterPoint().y > acPosPix.y)
				dc.LineTo(TagArea.CenterPoint().x, TagArea.TopLeft().y);
			else
				dc.LineTo(TagArea.CenterPoint().x, TagArea.BottomRight().y);

			dc.SelectObject(pqOrigPen);
			AddScreenObject(DRAWING_TAG, rt.GetCallsign(), TagArea, true, GetBottomLine(fp.GetCallsign()).c_str());

			dc.SetTextColor(RGB(255, 255, 255));
			int oldTextAlign = dc.SetTextAlign(TA_LEFT);
			string callsign = rt.GetCallsign();

			if (fp.IsValid()) {
				if (fp.GetCorrelatedRadarTarget().IsValid()) {
					if (fp.GetControllerAssignedData().GetCommunicationType() != ' ' && fp.GetControllerAssignedData().GetCommunicationType() != NULL) {
						if (fp.GetControllerAssignedData().GetCommunicationType() != 'v' && fp.GetControllerAssignedData().GetCommunicationType() != 'V') {
							callsign.append("/");
							callsign += fp.GetControllerAssignedData().GetCommunicationType();
						}
					}
					else if (fp.GetFlightPlanData().GetCommunicationType() == 't' || fp.GetFlightPlanData().GetCommunicationType() == 'r' || fp.GetFlightPlanData().GetCommunicationType() == 'T' || fp.GetFlightPlanData().GetCommunicationType() == 'R' && fp.GetControllerAssignedData().GetCommunicationType() != ' ' && fp.GetControllerAssignedData().GetCommunicationType() != NULL) {
						callsign.append("/");
						callsign += fp.GetFlightPlanData().GetCommunicationType();
					}
				}
				else {
					callsign = rt.GetPosition().GetSquawk();
				}

				switch (fp.GetState()) {

				case FLIGHT_PLAN_STATE_TRANSFER_TO_ME_INITIATED:
					callsign = ">>" + callsign;
					break;

				case FLIGHT_PLAN_STATE_TRANSFER_FROM_ME_INITIATED:
					callsign = callsign + ">>";
					break;

				}

				if ((fp.GetState() == FLIGHT_PLAN_STATE_TRANSFER_FROM_ME_INITIATED || fp.GetState() == FLIGHT_PLAN_STATE_TRANSFER_TO_ME_INITIATED) && BLINK) {
					if (fp.GetState() == FLIGHT_PLAN_STATE_TRANSFER_FROM_ME_INITIATED) {
						string blank = "";
						for (size_t i = 0; i <= strlen(callsign.c_str()) - 2; ++i) {
							blank.append(" ");
						}
						blank.append(">>");
						dc.TextOutA(TagArea.left + 2, TagArea.top + 1, blank.c_str());
					}

					if (fp.GetState() == FLIGHT_PLAN_STATE_TRANSFER_TO_ME_INITIATED) {
						dc.TextOutA(TagArea.left + 2, TagArea.top + 1, ">>");
					}
				}
				else {
					dc.TextOutA(TagArea.left + 2, TagArea.top + 1, callsign.c_str());
				}

				RECT CallsignArea = { TagArea.left + 2, TagArea.top + 1, (TagArea.left + 2) + dc.GetTextExtent(callsign.c_str()).cx, (TagArea.top + 1) + dc.GetTextExtent(callsign.c_str()).cy };
				AddScreenObject(TAG_CITEM_CALLSIGN, rt.GetCallsign(), CallsignArea, true, GetBottomLine(fp.GetCallsign()).c_str());

				const char * assr = fp.GetControllerAssignedData().GetSquawk();
				const char * ssr = rt.GetPosition().GetSquawk();
				bool has_squawk_error = false;
				string squawk_error;
				if (strlen(assr) != 0 && !startsWith(ssr, assr)) {
					has_squawk_error = true;
					squawk_error = "A";
					squawk_error.append(assr);
				}

				string to_display;
				if (has_squawk_error && BLINK) {
					dc.SetTextColor(RGB(255, 50, 50));
					to_display = squawk_error;
				}
				else {
					to_display = "A";
					int FL = RtPos.GetPressureAltitude();
					if (FL > GetPlugIn()->GetTransitionAltitude()) {
						to_display = "F";
						FL = RtPos.GetFlightLevel();
					}
					ostringstream strs;
					strs << FL;
					string str = strs.str();
					for (size_t i = 0; i < 6 - str.length(); i++)
						str = "0" + str;
					str.erase(str.begin() + 3, str.end());
					to_display.append(str);
					int delta_fl = FL - rt.GetPreviousPosition(RtPos).GetPressureAltitude();
					if (abs(delta_fl) >= 20) {
						if (delta_fl < 0) {
							to_display.append(" |");
						}
						else {
							to_display.append(" ^");
						}
					}

				}
				dc.TextOutA(TagArea.left + 2, TagArea.top + 14, to_display.c_str());
				dc.SetTextColor(RGB(255, 255, 255));
				RECT actArea = { TagArea.left + 2, TagArea.top + 14, (TagArea.left + 2) + dc.GetTextExtent(fp.GetFlightPlanData().GetAircraftFPType()).cx, (TagArea.top + 14) + dc.GetTextExtent(fp.GetFlightPlanData().GetAircraftFPType()).cy };
				AddScreenObject(TAG_CITEM_FPBOX, rt.GetCallsign(), actArea, true, GetBottomLine(fp.GetCallsign()).c_str());

			}

		}
	}

	CBrush BrushGrey(RGB(150, 150, 150));
	COLORREF oldColor = dc.SetTextColor(RGB(33, 33, 33));

	int TextHeight = dc.GetTextExtent("60").cy;

	for (std::map<string, bool>::iterator it = RimcasInstance->MonitoredRunwayArr.begin(); it != RimcasInstance->MonitoredRunwayArr.end(); ++it)
	{
		if (!it->second || RimcasInstance->TimeTable[it->first].empty())
			continue;

		int randArea = (rand() % 100 + 1);

		if (TimePopupAreas.find(it->first) == TimePopupAreas.end())
			TimePopupAreas[it->first] = TimePopupArea;

		CRect CRectTime = TimePopupAreas[it->first];
		CRectTime.NormalizeRect();

		dc.FillRect(CRectTime, &BrushGrey);

		map<int, int> times;

		times[1] = 60; 
		times[2] = 45; 
		times[3] = 30;
		times[4] = 15;

		if (RimcasInstance->RunwayTimerShort == false) {
			times[1] = 120; 
			times[2] = 90; 
			times[3] = 60; 
			times[4] = 30;
		}
			
		string tempS = it->first;
		dc.TextOutA(CRectTime.left + CRectTime.Width() / 2 - dc.GetTextExtent(tempS.c_str()).cx / 2, CRectTime.top, tempS.c_str());
		tempS = std::to_string(times[1]) + ": " + RimcasInstance->TimeTable[it->first][times[1]];
		dc.TextOutA(CRectTime.left, CRectTime.top + TextHeight, tempS.c_str());
		tempS = std::to_string(times[2]) + ": " + RimcasInstance->TimeTable[it->first][times[2]];
		dc.TextOutA(CRectTime.left, CRectTime.top + TextHeight * 2, tempS.c_str());
		tempS = std::to_string(times[3]) + ": " + RimcasInstance->TimeTable[it->first][times[3]];
		dc.SetTextColor(RGB(33, 33, 33));
		if (RimcasInstance->AcColor.find(RimcasInstance->TimeTable[it->first][30]) != RimcasInstance->AcColor.end() && RimcasInstance->RunwayTimerShort) {
			CBrush OrangeBrush(RimcasInstance->GetAircraftColor(RimcasInstance->TimeTable[it->first][30], RGB(0, 0, 0))); // RGB(180, 100, 50)
			CRect TempRect = { CRectTime.left, CRectTime.top + TextHeight * 3, CRectTime.right, CRectTime.top + TextHeight * 4 };
			dc.FillRect(TempRect, &OrangeBrush);
			dc.SetTextColor(RGB(238, 238, 208));
		}
		dc.TextOutA(CRectTime.left, CRectTime.top + TextHeight * 3, tempS.c_str());
		tempS = std::to_string(times[4]) + ": " + RimcasInstance->TimeTable[it->first][times[4]];
		dc.SetTextColor(RGB(33, 33, 33));
		if (RimcasInstance->AcColor.find(RimcasInstance->TimeTable[it->first][times[4]]) != RimcasInstance->AcColor.end()) {
			CBrush OrangeBrush(RimcasInstance->GetAircraftColor(RimcasInstance->TimeTable[it->first][times[4]], RGB(0, 0, 0)));
			CRect TempRect = { CRectTime.left, CRectTime.top + TextHeight * 4, CRectTime.right, CRectTime.top + TextHeight * 5 };
			dc.FillRect(TempRect, &OrangeBrush);
			dc.SetTextColor(RGB(238, 238, 208));
		}
		dc.TextOutA(CRectTime.left, CRectTime.top + TextHeight * 4, tempS.c_str());

		dc.SetTextColor(oldColor);

		AddScreenObject(7000, it->first.c_str(), CRectTime, true, "Runway timer");

	}

	if (ShowLists["CA Arrival"]) {
		GetPlugIn()->OpenPopupList(ListAreas["CA Arrival"], "CA Arrival", 1);
		for (std::map<string, CRimcas::RunwayAreaType>::iterator it = RimcasInstance->RunwayAreas.begin(); it != RimcasInstance->RunwayAreas.end(); ++it)
		{
			GetPlugIn()->AddPopupListElement(it->first.c_str(), "", RIMCAS_CA_ARRIVAL_FUNC, false, RimcasInstance->MonitoredRunwayArr[it->first.c_str()]);
		}
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		ShowLists["CA Arrival"] = false;
	}

	if (ShowLists["CA Monitor"]) {
		GetPlugIn()->OpenPopupList(ListAreas["CA Monitor"], "CA Monitor", 1);
		for (std::map<string, CRimcas::RunwayAreaType>::iterator it = RimcasInstance->RunwayAreas.begin(); it != RimcasInstance->RunwayAreas.end(); ++it)
		{
			GetPlugIn()->AddPopupListElement(it->first.c_str(), "", RIMCAS_CA_MONITOR_FUNC, false, RimcasInstance->MonitoredRunwayDep[it->first.c_str()]);
		}
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		ShowLists["CA Monitor"] = false;
	}

	if (ShowLists["Closed Runways"]) {
		GetPlugIn()->OpenPopupList(ListAreas["Closed Runways"], "Closed Runways", 1);
		for (std::map<string, CRimcas::RunwayAreaType>::iterator it = RimcasInstance->RunwayAreas.begin(); it != RimcasInstance->RunwayAreas.end(); ++it)
		{
			GetPlugIn()->AddPopupListElement(it->first.c_str(), "", RIMCAS_CLOSED_RUNWAYS_FUNC, false, RimcasInstance->ClosedRunway[it->first.c_str()]);
		}
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		ShowLists["Closed Runways"] = false;
	}

	if (ShowLists["Tags  >"]) {
		GetPlugIn()->OpenPopupList(ListAreas["Tags  >"], "Tags Menu", 1);
		GetPlugIn()->AddPopupListElement("Tag Colour 1", "", RIMCAS_TAGCOLOR, false, TagColorIsFirst);
		GetPlugIn()->AddPopupListElement("Tag Colour 2", "", RIMCAS_TAGCOLOR, false, !TagColorIsFirst);
		GetPlugIn()->AddPopupListElement("2nd line", "", RIMCAS_TAGS_2NDLINE, false, Display2ndLine);
		GetPlugIn()->AddPopupListElement("Hide a/c type", "", RIMCAS_TAGS_ACTYPE, false, HideAcType); 
		GetPlugIn()->AddPopupListElement("Display squawk warning", "", RIMCAS_TAGS_SQWARNING, false, DisplaySquawkWarning);
		GetPlugIn()->AddPopupListElement("Display aspeed for gate", "", RIMCAS_TAGS_SPEEDGATE, false, UseSpeedForGate);
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		ShowLists["Tags  >"] = false;
	}


	//---------------------------------
	// QRD
	//---------------------------------

	if (QDMenabled) {
		CPen Pen(PS_SOLID, 1, RGB(255, 255, 255));
		CPen *oldPen = dc.SelectObject(&Pen);

		POINT AirportPos = ConvertCoordFromPositionToPixel(AirportPositions[RimcasInstance->ActiveAirport]);
		dc.MoveTo(AirportPos);
		POINT p;
		if (GetCursorPos(&p))
		{
			dc.LineTo(p);

			CPosition CursorPos = ConvertCoordFromPixelToPosition(p);
			double Distance = AirportPositions[RimcasInstance->ActiveAirport].DistanceTo(CursorPos);
			double Bearing = AirportPositions[RimcasInstance->ActiveAirport].DirectionTo(CursorPos);

			TGraphics th;
			th.DrawEllipse(dc, p.x - 5, p.y - 5, p.x + 5, p.y + 5, RGB(255, 255, 255));

			Distance = Distance / 0.00053996f;

			Distance = round(Distance * 10) / 10;

			Bearing = round(Bearing * 10) / 10;

			POINT TextPos = { p.x + 20, p.y };

			string distances = std::to_string(Distance);
			size_t decimal_pos = distances.find(".");
			distances = distances.substr(0, decimal_pos + 2);

			string bearings = std::to_string(Bearing);
			decimal_pos = bearings.find(".");
			bearings = bearings.substr(0, decimal_pos + 2);

			string text = distances;
			text += "m ";
			text += bearings;
			text += "°";
			COLORREF oldColor = dc.SetTextColor(RGB(255, 255, 255));
			dc.TextOutA(TextPos.x, TextPos.y, text.c_str());
			dc.SetTextColor(oldColor);
		}

		dc.SelectObject(oldPen);
	
	}

	//---------------------------------
	// Drawing the toolbar
	//---------------------------------

	COLORREF qToolBarColor = RGB(127, 122, 122);

	// Drawing the toolbar on the top
	CRect ToolBarAreaTop(RadarArea.left, RadarArea.top, RadarArea.right, RadarArea.top + 20);
	dc.FillSolidRect(ToolBarAreaTop, qToolBarColor);

	COLORREF oldTextColor = dc.SetTextColor(RGB(0, 0, 0));

	int offset = 2;
	dc.TextOutA(ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, RimcasInstance->ActiveAirport.c_str());
	AddScreenObject(7999, "ActiveAirport", { ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, ToolBarAreaTop.left + offset + dc.GetTextExtent(RimcasInstance->ActiveAirport.c_str()).cx, ToolBarAreaTop.top + 4 + dc.GetTextExtent(RimcasInstance->ActiveAirport.c_str()).cy }, false, "Active Airport");

	offset += dc.GetTextExtent(RimcasInstance->ActiveAirport.c_str()).cx + 10;
	dc.TextOutA(ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, "Display");
	AddScreenObject(8000, "DisplayMenu", { ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, ToolBarAreaTop.left + offset + dc.GetTextExtent("Display").cx, ToolBarAreaTop.top + 4 + dc.GetTextExtent("Display").cy }, false, "Display menu");

	offset += dc.GetTextExtent("Display").cx + 10;
	dc.TextOutA(ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, "RIMCAS");
	AddScreenObject(8001, "RIMCASMenu", { ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, ToolBarAreaTop.left + offset + dc.GetTextExtent("RIMCAS").cx, ToolBarAreaTop.top + 4 + +dc.GetTextExtent("RIMCAS").cy }, false, "RIMCAS menu");

	dc.SetTextColor(oldTextColor);

	//
	// App window
	//

	if (appWindow) {
		COLORREF qBackgroundColor = RGB(127, 122, 122);
		CRect windowAreaCRect(appWindowArea);
		windowAreaCRect.NormalizeRect();
		dc.FillSolidRect(windowAreaCRect, qBackgroundColor);
		AddScreenObject(8888, "window", appWindowArea, true, "");
		
		int scale = appWindowScale; // 10pix for 1nm

		POINT refPt = windowAreaCRect.CenterPoint();

		for (std::map<string, CRimcas::RunwayAreaType>::iterator it = RimcasInstance->RunwayAreas.begin(); it != RimcasInstance->RunwayAreas.end(); ++it)
		{
			CPosition pt1, pt2;
			pt1 = Haversine(it->second.topLeft, RadToDeg(float(TrueBearing(it->second.topLeft, it->second.bottomLeft))), float((it->second.topLeft.DistanceTo(it->second.bottomLeft) / 2) / 0.00053996));
			pt2 = Haversine(it->second.topRight, RadToDeg(float(TrueBearing(it->second.topRight, it->second.bottomRight))), float((it->second.topRight.DistanceTo(it->second.bottomRight) / 2) / 0.00053996));
			double dist, dir;

			POINT t1s, t2s;

			dist = AirportPositions[RimcasInstance->ActiveAirport].DistanceTo(pt1);
			dir = TrueBearing(AirportPositions[RimcasInstance->ActiveAirport], pt1);

			t1s.x = refPt.x + int(scale * dist * sin(dir) + 0.5);
			t1s.y = refPt.y - int(scale * dist * cos(dir) + 0.5);

			dist = AirportPositions[RimcasInstance->ActiveAirport].DistanceTo(pt2);
			dir = TrueBearing(AirportPositions[RimcasInstance->ActiveAirport], pt2);

			t2s.x = refPt.x + int(scale * dist * sin(dir) + 0.5);
			t2s.y = refPt.y - int(scale * dist * cos(dir) + 0.5);

			CPen PenT(PS_SOLID, 1, RGB(0, 0, 0));
			dc.SelectObject(PenT);
			POINT toDraw1, toDraw2;
			if (LiangBarsky(appWindowArea, t1s, t2s, toDraw1, toDraw2)) {
				dc.MoveTo(toDraw1);
				dc.LineTo(toDraw2);
			}

			// Extended centerlines
			if (RimcasInstance->MonitoredRunwayArr[it->first]) {
				dist = AirportPositions[RimcasInstance->ActiveAirport].DistanceTo(it->second.threshold);
				dir = TrueBearing(AirportPositions[RimcasInstance->ActiveAirport], it->second.threshold);

				t1s.x = refPt.x + int(scale * dist * sin(dir) + 0.5);
				t1s.y = refPt.y - int(scale * dist * cos(dir) + 0.5);
				CPen PenC(PS_SOLID, 1, RGB(255, 255, 255));
				dc.SelectObject(PenC);

				double revHdg = it->second.bearing;
				double exsize = 18520.0;
				CPosition Far = Haversine(it->second.threshold, RadToDeg(float(revHdg)), exsize);
				POINT pt;
				revHdg = TrueBearing(AirportPositions[RimcasInstance->ActiveAirport], Far);
				exsize = AirportPositions[RimcasInstance->ActiveAirport].DistanceTo(Far);
				pt.x = refPt.x + int(scale * exsize * sin(revHdg) + 0.5);
				pt.y = refPt.y - int(scale * exsize * cos(revHdg) + 0.5);

				if (LiangBarsky(appWindowArea, t1s, pt, toDraw1, toDraw2)) {
					dc.MoveTo(toDraw1);
					dc.LineTo(toDraw2);
				}
				
				for (int j = 1852; j <= 18520; j += 1852) {
					
					revHdg = it->second.bearing;
					exsize = j;
					CPosition Far1 = Haversine(it->second.threshold, RadToDeg(float(revHdg)), exsize);
					CPosition Far2 = Haversine(Far1, fmod(RadToDeg(float(revHdg)) - 90, 360), 500);
					CPosition Far3 = Haversine(Far1, fmod(RadToDeg(float(revHdg)) + 90, 360), 500);
					POINT pt2, pt3;
					revHdg = TrueBearing(AirportPositions[RimcasInstance->ActiveAirport], Far2);
					exsize = AirportPositions[RimcasInstance->ActiveAirport].DistanceTo(Far2);
					pt2.x = refPt.x + int(scale * exsize * sin(revHdg) + 0.5);
					pt2.y = refPt.y - int(scale * exsize * cos(revHdg) + 0.5);

					revHdg = TrueBearing(AirportPositions[RimcasInstance->ActiveAirport], Far3);
					exsize = AirportPositions[RimcasInstance->ActiveAirport].DistanceTo(Far3);
					pt3.x = refPt.x + int(scale * exsize * sin(revHdg) + 0.5);
					pt3.y = refPt.y - int(scale * exsize * cos(revHdg) + 0.5);
					
					if (LiangBarsky(appWindowArea, pt2, pt3, toDraw1, toDraw2)) {
						dc.MoveTo(toDraw1);
						dc.LineTo(toDraw2);
					}

				}

			}

		}

		// Aircrafts

		CRect windowAreaRect2(appWindowArea);

		vector<POINT> appAreaVect = { windowAreaRect2.TopLeft(), { windowAreaRect2.right, windowAreaRect2.top }, windowAreaRect2.BottomRight(), { windowAreaRect2.left, windowAreaRect2.bottom } };
		CPen WhitePen(PS_SOLID, 1, RGB(255, 255, 255));

		for (rt = GetPlugIn()->RadarTargetSelectFirst();
			rt.IsValid();
			rt = GetPlugIn()->RadarTargetSelectNext(rt))
		{
			if (rt.GetGS() < 50 || rt.GetPosition().GetPressureAltitude() > appWindowFilter || !rt.IsValid() || !rt.GetPosition().IsValid())
				continue;

			CPosition RtPos2 = rt.GetPosition().GetPosition();
			POINT RtPoint, hPoint;
			double hdg, dist;

			hdg = TrueBearing(AirportPositions[RimcasInstance->ActiveAirport], RtPos2);
			dist = AirportPositions[RimcasInstance->ActiveAirport].DistanceTo(RtPos2);
			RtPoint.x = refPt.x + int(scale * dist * sin(hdg) + 0.5);
			RtPoint.y = refPt.y - int(scale * dist * cos(hdg) + 0.5);

			CRadarTargetPositionData hPos = rt.GetPreviousPosition(rt.GetPosition());
			for (int i = 1; i < 4; i++) {
				if (!hPos.IsValid())
					continue;

				hdg = TrueBearing(AirportPositions[RimcasInstance->ActiveAirport], hPos.GetPosition());
				dist = AirportPositions[RimcasInstance->ActiveAirport].DistanceTo(hPos.GetPosition());
				hPoint.x = refPt.x + int(scale * dist * sin(hdg) + 0.5);
				hPoint.y = refPt.y - int(scale * dist * cos(hdg) + 0.5);

				if (Is_Inside(hPoint, appAreaVect)) {
					dc.SetPixel(hPoint, RGB(255, 255, 255));
				}
				
				hPos = rt.GetPreviousPosition(hPos);
			}

			if (Is_Inside(RtPoint, appAreaVect)) {
				dc.SelectObject(&WhitePen);
				dc.MoveTo({ RtPoint.x, RtPoint.y - 4 });
				dc.LineTo({ RtPoint.x - 4, RtPoint.y });
				dc.LineTo({ RtPoint.x, RtPoint.y + 4 });
				dc.LineTo({ RtPoint.x + 4, RtPoint.y });
				dc.LineTo({ RtPoint.x, RtPoint.y - 4 });
			}

			int lenght = 15;

			POINT TopLeftText;
			TopLeftText.x = long(RtPoint.x + float(lenght * cos(DegToRad(45))));
			TopLeftText.y = long(RtPoint.y + float(lenght * sin(DegToRad(45))));

			string callsign = rt.GetCallsign();
			CFlightPlan fp = rt.GetCorrelatedFlightPlan();

			if (fp.IsValid()) {
				if (fp.GetControllerAssignedData().GetCommunicationType() != ' ' && fp.GetControllerAssignedData().GetCommunicationType() != NULL) {
					if (fp.GetControllerAssignedData().GetCommunicationType() != 'v' && fp.GetControllerAssignedData().GetCommunicationType() != 'V') {
						callsign.append("/");
						callsign += fp.GetControllerAssignedData().GetCommunicationType();
					}
				}
				else if (fp.GetFlightPlanData().GetCommunicationType() == 't' || fp.GetFlightPlanData().GetCommunicationType() == 'r' || fp.GetFlightPlanData().GetCommunicationType() == 'T' || fp.GetFlightPlanData().GetCommunicationType() == 'R' && fp.GetControllerAssignedData().GetCommunicationType() != ' ' && fp.GetControllerAssignedData().GetCommunicationType() != NULL) {
					callsign.append("/");
					callsign += fp.GetFlightPlanData().GetCommunicationType();
				}

				switch (fp.GetState()) {

				case FLIGHT_PLAN_STATE_TRANSFER_TO_ME_INITIATED:
					callsign = ">>" + callsign;
					break;

				case FLIGHT_PLAN_STATE_TRANSFER_FROM_ME_INITIATED:
					callsign = callsign + ">>";
					break;

				}
			}
			else {
				if (rt.GetPosition().GetRadarFlags() >= RADAR_POSITION_SECONDARY_S)
					callsign = "@" + callsign;
				else
					callsign = rt.GetPosition().GetSquawk();
			}

			CRadarTargetPositionData RtPos = rt.GetPosition();
			string to_display;

			to_display = "A";
			int FL = RtPos.GetPressureAltitude();
			if (FL > GetPlugIn()->GetTransitionAltitude()) {
				to_display = "F";
				FL = RtPos.GetFlightLevel();
			}
			ostringstream strs;
			strs << FL;
			string str = strs.str();
			for (size_t i = 0; i < 6 - str.length(); i++)
				str = "0" + str;
			str.erase(str.begin() + 3, str.end());
			to_display.append(str);
			int delta_fl = FL - rt.GetPreviousPosition(RtPos).GetPressureAltitude();
			if (abs(delta_fl) >= 20) {
				if (delta_fl < 0) {
					to_display.append(" |");
				}
				else {
					to_display.append(" ^");
				}
			}

			CRect TagBottomRight(Utils::GetAreaFromText(&dc, to_display.c_str(), { TopLeftText.x, TopLeftText.y + dc.GetTextExtent(callsign.c_str()).cy }));

			if (Is_Inside(TopLeftText, appAreaVect) && Is_Inside(RtPoint, appAreaVect) && Is_Inside(TagBottomRight.BottomRight(), appAreaVect) && Is_Inside(TagBottomRight.TopLeft(), appAreaVect)) {
				dc.SelectObject(&WhitePen);
				dc.MoveTo(RtPoint);
				dc.LineTo(TopLeftText);
				COLORREF oldTextC = dc.SetTextColor(RGB(255, 255, 255));
				dc.TextOutA(TopLeftText.x, TopLeftText.y, callsign.c_str());
				TopLeftText.y += dc.GetTextExtent(callsign.c_str()).cy;
				dc.TextOutA(TopLeftText.x, TopLeftText.y, to_display.c_str());
				dc.SetTextColor(oldTextC);
			}
			
		}

		// Resize square
		qBackgroundColor = RGB(60, 60, 60);
		POINT BottomRight = { appWindowArea.right, appWindowArea.bottom };
		POINT TopLeft = { BottomRight.x - 10, BottomRight.y - 10 };
		CRect ResizeArea = { TopLeft, BottomRight };
		ResizeArea.NormalizeRect();
		dc.FillSolidRect(ResizeArea, qBackgroundColor);
		AddScreenObject(8888, "resize", ResizeArea, true, "");

		CPen DarkPenResize(PS_SOLID, 1, RGB(0, 0, 0));
		dc.SelectObject(&DarkPenResize);
		dc.MoveTo(ResizeArea.TopLeft());
		dc.LineTo({ResizeArea.left, ResizeArea.bottom});
		dc.MoveTo(ResizeArea.TopLeft());
		dc.LineTo({ ResizeArea.right, ResizeArea.top });

		// Sides
		CBrush FrameBrush(RGB(35, 35, 35));
		dc.FrameRect(windowAreaCRect, &FrameBrush);

		// Topbar
		TopLeft = windowAreaCRect.TopLeft();
		TopLeft.y = TopLeft.y - 15;
		BottomRight = { windowAreaCRect.right, windowAreaCRect.top };
		CRect TopBar(TopLeft, BottomRight);
		TopBar.NormalizeRect();
		dc.FillRect(TopBar, &FrameBrush);
		POINT TopLeftText = { TopBar.left + 5, TopBar.bottom - dc.GetTextExtent("Approach Window (zoom: 10, filter: 6000ft)").cy };
		COLORREF oldTextColorC = dc.SetTextColor(RGB(238, 238, 208));
		
		AddScreenObject(8888, "topbar", TopBar, true, "");

		string Toptext = "Approach Window (zoom: ";
		Toptext += std::to_string(appWindowScale);
		Toptext += ", filter: ";
		Toptext += std::to_string(appWindowFilter);
		Toptext += "ft)";
		dc.TextOutA(TopLeftText.x, TopLeftText.y, Toptext.c_str());

		int TextOffset = dc.GetTextExtent("Approach Window (zoom: ").cx;
		string temp = std::to_string(appWindowScale);
		AddScreenObject(8888, "range", Utils::GetAreaFromText(&dc, temp.c_str(), { TopLeftText.x + TextOffset, TopLeftText.y }), false, "");
		
		TextOffset += dc.GetTextExtent(temp.c_str()).cx;
		TextOffset += dc.GetTextExtent(", filter: ").cx;
		temp = std::to_string(appWindowFilter);
		temp += "ft";
		AddScreenObject(8888, "filter", Utils::GetAreaFromText(&dc, temp.c_str(), { TopLeftText.x + TextOffset, TopLeftText.y }), false, "");

		dc.SetTextColor(oldTextColorC);

		// Close
		POINT TopLeftClose = { TopBar.right - 16, TopBar.top + 2 };
		POINT BottomRightClose = { TopBar.right - 5, TopBar.bottom - 2 };
		CRect CloseRect(TopLeftClose, BottomRightClose);
		CloseRect.NormalizeRect();
		CBrush CloseBrush(RGB(60, 60, 60));
		dc.FillRect(CloseRect, &CloseBrush);
		CPen BlackPen(PS_SOLID, 1, RGB(0, 0, 0));
		dc.SelectObject(BlackPen);
		dc.MoveTo(CloseRect.TopLeft());
		dc.LineTo(CloseRect.BottomRight());
		dc.MoveTo({ CloseRect.right - 1, CloseRect.top });
		dc.LineTo({ CloseRect.left - 1, CloseRect.bottom });
		AddScreenObject(8888, "close", CloseRect, false, "");

	}

	dc.Detach();

}
