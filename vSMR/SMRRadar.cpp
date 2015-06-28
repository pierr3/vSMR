#include "stdafx.h"
#include "SMRRadar.hpp"

RECT TimePopupArea = { 300, 300, 430, 363 };

ULONG_PTR m_gdiplusToken;
int currentFontSize = 1;
map<int, Gdiplus::Font *> customFonts;

CSMRRadar::CSMRRadar()
{
	clock_init = clock();
	srand((unsigned)time(0));

	// Initialize GDI+
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	GetModuleFileNameA((HINSTANCE)&__ImageBase, DllPathFile, sizeof(DllPathFile));
	DllPath = DllPathFile;
	DllPath.resize(DllPath.size() - strlen("vSMR.dll"));

	Callsigns = new CCallsignLookup(DllPath + "\\ICAO_Airlines.txt");

	if (RimcasInstance == NULL)
		RimcasInstance = new CRimcas();

	if (CurrentConfig == NULL)
		CurrentConfig = new CConfig(DllPath + "\\vSMR_Profiles.json");

	MenuPositions["DisplayMenu"] = { 400, 400, 550, 500 };

	appWindowArea = { 200, 200, 600, 500 };

	appWindowFilter = 6000;
	appWindowScale = 15;

	LoadCustomFont();

	RefreshAirportActivity();
}

CSMRRadar::~CSMRRadar()
{
	GdiplusShutdown(m_gdiplusToken);
}

void CSMRRadar::LoadCustomFont() {
	// Loading the custom font if there is one in use
	customFonts.clear();
	RimcasInstance->toggleShortTimer(CurrentConfig->getActiveProfile()["rimcas"]["short_timer"].GetBool());
	if (CurrentConfig->getActiveProfile()["font"]["use_custom_font"].GetBool()) {
		const Value& FSizes = CurrentConfig->getActiveProfile()["font"]["sizes"];
		string font_name = CurrentConfig->getActiveProfile()["font"]["font_name"].GetString();
		wstring buffer = wstring(font_name.begin(), font_name.end());
		Gdiplus::FontStyle fontStyle = Gdiplus::FontStyleRegular;
		if (strcmp(CurrentConfig->getActiveProfile()["font"]["weight"].GetString(), "Bold") == 0)
			fontStyle = Gdiplus::FontStyleBold;
		if (strcmp(CurrentConfig->getActiveProfile()["font"]["weight"].GetString(), "Italic") == 0)
			fontStyle = Gdiplus::FontStyleItalic;
		customFonts[1] = new Gdiplus::Font(buffer.c_str(), FSizes["one"].GetInt(), fontStyle, Gdiplus::UnitPixel);
		customFonts[2] = new Gdiplus::Font(buffer.c_str(), FSizes["two"].GetInt(), fontStyle, Gdiplus::UnitPixel);
		customFonts[3] = new Gdiplus::Font(buffer.c_str(), FSizes["three"].GetInt(), fontStyle, Gdiplus::UnitPixel);
		customFonts[4] = new Gdiplus::Font(buffer.c_str(), FSizes["four"].GetInt(), fontStyle, Gdiplus::UnitPixel);
	}
}

void CSMRRadar::OnAsrContentLoaded(bool Loaded)
{
	const char * p_value;

	if ((p_value = GetDataFromAsr("Airport")) != NULL)
		setActiveAirport(p_value);

	if ((p_value = GetDataFromAsr("ShortTimer")) != NULL)
	{
		int temp = atoi(p_value);
		RimcasInstance->RunwayTimerShort = false;
		if (temp == 1)
			RimcasInstance->RunwayTimerShort = true;
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

	// Auto load the airport config on ASR opened.
	CSectorElement rwy;
	for (rwy = GetPlugIn()->SectorFileElementSelectFirst(SECTOR_ELEMENT_RUNWAY);
		rwy.IsValid();
		rwy = GetPlugIn()->SectorFileElementSelectNext(rwy, SECTOR_ELEMENT_RUNWAY))
	{
		if (startsWith(getActiveAirport().c_str(), rwy.GetAirportName())) {
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
	SaveDataToAsr("Airport", "Active airport for RIMCAS", getActiveAirport().c_str());

	const char * to_save = "0";
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
	if (RimcasInstance->RunwayTimerShort)
		to_save = "1";
	SaveDataToAsr("ShortTimer", "Timer lenght", to_save);

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
			GetPlugIn()->OpenPopupEdit(Area, RIMCAS_UPDATERANGE, std::to_string(appWindowScale).c_str());
		}
		if (strcmp(sObjectId, "filter") == 0) {
			GetPlugIn()->OpenPopupList(Area, "Filter", 1);
			GetPlugIn()->AddPopupListElement("66000", "", RIMCAS_UPDATEFILTER);
			GetPlugIn()->AddPopupListElement("24500", "", RIMCAS_UPDATEFILTER);
			GetPlugIn()->AddPopupListElement("8000", "", RIMCAS_UPDATEFILTER);
			GetPlugIn()->AddPopupListElement("6000", "", RIMCAS_UPDATEFILTER);
			GetPlugIn()->AddPopupListElement("3000", "", RIMCAS_UPDATEFILTER);
			GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		}
	}

	if (ObjectType == RIMCAS_ACTIVE_AIRPORT) {
		GetPlugIn()->OpenPopupEdit(Area, RIMCAS_ACTIVE_AIRPORT_FUNC, getActiveAirport().c_str());
	}

	if (ObjectType == RIMCAS_DISPLAY_MENU) {

		Area.top = Area.top + 30;
		Area.bottom = Area.bottom + 30;

		GetPlugIn()->OpenPopupList(Area, "Display Menu", 1);
		GetPlugIn()->AddPopupListElement("QDR", "", RIMCAS_QDM_TOGGLE);
		GetPlugIn()->AddPopupListElement("Inset 1", "", RIMCAS_APPWINDOW);
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
	}

	if (ObjectType == RIMCAS_RIMCAS_MENU) {

		Area.top = Area.top + 30;
		Area.bottom = Area.bottom + 30;

		GetPlugIn()->OpenPopupList(Area, "Alerts", 1);
		GetPlugIn()->AddPopupListElement("Conflict Alert ARR", "", RIMCAS_OPEN_LIST);
		GetPlugIn()->AddPopupListElement("Conflict Alert DEP", "", RIMCAS_OPEN_LIST);
		GetPlugIn()->AddPopupListElement("Runway closed", "", RIMCAS_OPEN_LIST);
		GetPlugIn()->AddPopupListElement("Visibility", "", RIMCAS_OPEN_LIST);
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
		setActiveAirport(sItemString);
		SaveDataToAsr("Airport", "Active airport", getActiveAirport().c_str());
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

	if (FunctionId == RIMCAS_CA_ARRIVAL_FUNC) {
		RimcasInstance->toggleMonitoredRunwayArr(string(sItemString));

		ShowLists["Conflict Alert ARR"] = true;

		RequestRefresh();
	}

	if (FunctionId == RIMCAS_CA_MONITOR_FUNC) {
		RimcasInstance->toggleMonitoredRunwayDep(string(sItemString));

		ShowLists["Conflict Alert DEP"] = true;

		RequestRefresh();
	}

	if (FunctionId == RIMCAS_CLOSED_RUNWAYS_FUNC) {
		RimcasInstance->toggleClosedRunway(string(sItemString));

		ShowLists["Closed Runways"] = true;

		RequestRefresh();
	}

	if (FunctionId == RIMCAS_OPEN_LIST) {

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

	// What we do is a create an elispe that represent the aircraft
	// The ellipse is defined in meters, and is only used to 
	// get the distance from the center to the point, that we randomize.

	//
	// Sometimes, coding can go weird: https://db.tt/FLHf0zWh
	//

	width = width + float((rand() % 5) - 2);
	cabin_width = cabin_width + float((rand() % 5) - 2);
	lenght = lenght + float((rand() % 5) - 2);

	double ry = lenght / 2.0; // ellipse radius y
	double rx = cabin_width / 2.0; // ellipse radius x

	double x0 = 0, y0 = 0; // ellipse params
	double a;

	int j = 0;
	for (a = 0.0; a<2.0*Pi;) // full ellipse
	{
		double xA, yA;

		xA = ry * cos(a);
		yA = ry * sin(a);

		POINT tP = { xA, yA*(rx / ry) };

		a += DegToRad(5.0 + (rand() % 10 + 1)); // random angle step < 5,10 > degrees

		double distance = abs(sqrt(pow(tP.x, 2.0) - pow(tP.y, 2.0)));
		if (distance > 0) {
			double hdg = fmod((double)RadToDeg(a) + (double)RtPos.GetReportedHeading(), 360);
			CPosition TempPos = Haversine(RtPos.GetPosition(), hdg, distance);
			Patatoides[RadarTarget.GetCallsign()].points[j] = { TempPos.m_Latitude, TempPos.m_Longitude };
			j++;
		}
	}
	/*

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

	*/
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
	if (strcmp(sCommandLine, ".smr reload") == 0) {
		CurrentConfig = new CConfig(DllPath + "\\vSMR_Profiles.json");
		LoadCustomFont();
		return true;
	}

	return false;
}

inline int Is_Left(const POINT &p0, const POINT &p1, const POINT &point)
{
	return ((p1.x - p0.x) * (point.y - p0.y) -
		(point.x - p0.x) * (p1.y - p0.y));
};

inline bool Is_Inside(const POINT &point, const std::vector<POINT> &points_list)
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
};

// Liang-Barsky function by Daniel White @ http://www.skytopia.com/project/articles/compsci/clipping.html
// This function inputs 8 numbers, and outputs 4 new numbers (plus a boolean value to say whether the clipped line is drawn at all).
//
inline bool LiangBarsky(RECT Area, POINT fromSrc, POINT toSrc, POINT &ClipFrom, POINT &ClipTo)         // The output values, so declare these outside.
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
};

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
		if (startsWith(getActiveAirport().c_str(), rwy.GetAirportName())) {

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

#pragma region symbols
	// Drawing the symbols
	EuroScopePlugIn::CRadarTarget rt;
	for (rt = GetPlugIn()->RadarTargetSelectFirst();
		rt.IsValid();
		rt = GetPlugIn()->RadarTargetSelectNext(rt))
	{
		if (!rt.IsValid() || !rt.GetPosition().IsValid())
			continue;

		int reportedGs = rt.GetPosition().GetReportedGS();
		int radarRange = CurrentConfig->getActiveProfile()["filters"]["radar_range_nm"].GetInt();
		int altitudeFilter = CurrentConfig->getActiveProfile()["filters"]["hide_above_alt"].GetInt();
		int speedFilter = CurrentConfig->getActiveProfile()["filters"]["hide_above_spd"].GetInt();
		bool isAcDisplayed = true;

		if (AirportPositions[getActiveAirport()].DistanceTo(rt.GetPosition().GetPosition()) > radarRange)
			isAcDisplayed = false;

		if (altitudeFilter != 0) {
			if (rt.GetPosition().GetPressureAltitude() > altitudeFilter)
				isAcDisplayed = false;
		}

		if (speedFilter != 0) {
			if (reportedGs > speedFilter)
				isAcDisplayed = false;
		}

		if (!isAcDisplayed)
			continue;

		RimcasInstance->OnRefresh(rt, this);

		CRadarTargetPositionData RtPos = rt.GetPosition();

		POINT acPosPix = ConvertCoordFromPositionToPixel(RtPos.GetPosition());

		TGraphics th;

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

				if (i == 1 && !Patatoides[rt.GetCallsign()].History_one_points.empty() && CurrentConfig->getActiveProfile()["targets"]["show_primary_target"].GetBool()) {
					CBrush* pOldBrush;
					CBrush brush;
					brush.CreateSolidBrush(SMR_H1_COLOR);
					pOldBrush = dc.SelectObject(&brush);
					CPen pen(PS_SOLID, 1, SMR_H1_COLOR);
					CPen* pOldPen = dc.SelectObject(&pen);
					CPoint lpPoints[100];
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
					if (!Patatoides[rt.GetCallsign()].History_two_points.empty() && CurrentConfig->getActiveProfile()["targets"]["show_primary_target"].GetBool()) {
						CBrush brush;
						brush.CreateSolidBrush(SMR_H2_COLOR);
						CBrush* pOldBrush;
						pOldBrush = dc.SelectObject(&brush);
						CPen pen(PS_SOLID, 1, SMR_H2_COLOR);
						CPen* pOldPen = dc.SelectObject(&pen);
						CPoint lpPoints[100];
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

				if (i == 2 && !Patatoides[rt.GetCallsign()].History_three_points.empty() && CurrentConfig->getActiveProfile()["targets"]["show_primary_target"].GetBool()) {
					CBrush brush;
					brush.CreateSolidBrush(SMR_H3_COLOR);
					CBrush* pOldBrush;
					pOldBrush = dc.SelectObject(&brush);
					CPen pen(PS_SOLID, 1, SMR_H3_COLOR);
					CPen* pOldPen = dc.SelectObject(&pen);
					CPoint lpPoints[100];
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

		if (CurrentConfig->getActiveProfile()["targets"]["show_primary_target"].GetBool()) {
			CBrush brush;
			brush.CreateSolidBrush(SMR_TARGET_COLOR);
			CBrush* pOldBrush;
			pOldBrush = dc.SelectObject(&brush);
			CPen pen(PS_SOLID, 1, SMR_TARGET_COLOR);
			CPen* pOldPen = dc.SelectObject(&pen);
			CPoint lpPoints[100];
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

#pragma endregion Drawing of the symbols

	TimePopupData.clear();
	AcOnRunway.clear();
	ColorAC.clear();

	RimcasInstance->OnRefreshEnd();

	// Creating the gdi+ graphics
	Graphics graphics(hDC);
	graphics.SetPageUnit(Gdiplus::UnitPixel);

	//graphics.SetSmoothingMode(SmoothingModeAntiAlias);

	// Get Device DPI Resolutions //
	int nLogPx = ::GetDeviceCaps(hDC, LOGPIXELSX);
	// Get GDI+ resolution
	int nGdiPlusLogPx = (int)graphics.GetDpiX();
	// set to pixels
	graphics.SetPageUnit(UnitPixel);
	// Adjust to match
	graphics.SetPageScale(((REAL)nGdiPlusLogPx / (REAL)nLogPx));

#pragma region tags
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
		int reportedGs = RtPos.GetReportedGS();

		// Filtering the targets

		int radarRange = CurrentConfig->getActiveProfile()["filters"]["radar_range_nm"].GetInt();
		int altitudeFilter = CurrentConfig->getActiveProfile()["filters"]["hide_above_alt"].GetInt();
		int speedFilter = CurrentConfig->getActiveProfile()["filters"]["hide_above_spd"].GetInt();
		bool isAcDisplayed = true;

		if (AirportPositions[getActiveAirport()].DistanceTo(RtPos.GetPosition()) > radarRange)
			isAcDisplayed = false;

		if (altitudeFilter != 0) {
			if (RtPos.GetPressureAltitude() > altitudeFilter)
				isAcDisplayed = false;
		}

		if (speedFilter != 0) {
			if (reportedGs > speedFilter)
				isAcDisplayed = false;
		}

		if (!isAcDisplayed)
			continue;

		// Getting the tag center/offset

		POINT TagCenter;
		map<string, POINT>::iterator it = TagsOffsets.find(rt.GetCallsign());
		if (it != TagsOffsets.end()) {
			TagCenter = { acPosPix.x + it->second.x, acPosPix.y + it->second.y };
		}
		else {
			TagCenter = { acPosPix.x + 35, acPosPix.y - 40 };
		}

		//----
		// Tag types
		//---

		static enum TagTypes { Departure, Arrival, Airborne };

		TagTypes TagType = TagTypes::Departure;
			
		if (fp.IsValid() && strcmp(fp.GetFlightPlanData().GetDestination(), getActiveAirport().c_str()) == 0) {
			TagType = TagTypes::Arrival;
		}

		if (reportedGs > 50) {
			TagType = TagTypes::Airborne;
		}

		// ----
		// Tag items available
		// callsign: Callsign with freq state and comm *
		// actype: Aircraft type *
		// sctype: Aircraft type that changes for squawk error *
		// sqerror: Squawk error if there is one, or empty *
		// deprwy: Departure runway *
		// seprwy: Departure runway that changes to speed if speed > 25kts *
		// gate: Gate, from speed or scratchpad *
		// sate: Gate, from speed or scratchpad that changes to speed if speed > 25kts *
		// flightlevel: Flightlevel/Pressure altitude of the ac *
		// gs: Ground speed of the ac *
		// tendency: Climbing or descending symbol *
		// wake: Wake turbulance cat *
		// ----

		// ----- Callsign -------
		string callsign = rt.GetCallsign();
		if (fp.IsValid()) {
			if (fp.GetControllerAssignedData().GetCommunicationType() == 't' || 
				fp.GetControllerAssignedData().GetCommunicationType() == 'T' || 
				fp.GetControllerAssignedData().GetCommunicationType() == 'r' || 
				fp.GetControllerAssignedData().GetCommunicationType() == 'R' ||
				fp.GetControllerAssignedData().GetCommunicationType() == 'v' ||
				fp.GetControllerAssignedData().GetCommunicationType() == 'V') 
			{
				if (fp.GetControllerAssignedData().GetCommunicationType() != 'v' &&
					fp.GetControllerAssignedData().GetCommunicationType() != 'V') {
					callsign.append("/");
					callsign += fp.GetControllerAssignedData().GetCommunicationType();
				}
			}
			else if (fp.GetFlightPlanData().GetCommunicationType() == 't' || 
				fp.GetFlightPlanData().GetCommunicationType() == 'r' || 
				fp.GetFlightPlanData().GetCommunicationType() == 'T' || 
				fp.GetFlightPlanData().GetCommunicationType() == 'R') 
			{
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
				
		// ----- Squawn error -------
		string sqerror = "";
		const char * assr = fp.GetControllerAssignedData().GetSquawk();
		const char * ssr = rt.GetPosition().GetSquawk();
		bool has_squawk_error = false;
		if (strlen(assr) != 0 && !startsWith(ssr, assr)) {
			has_squawk_error = true;
			sqerror = "A";
			sqerror.append(assr);
		}

		// ----- Aircraft type -------
		string actype = fp.GetFlightPlanData().GetAircraftFPType();
		if (actype.size() > 4)
			actype = actype.substr(0, 4);

		// ----- Aircraft type that changes to squawk error -------
		string sctype = actype;
		if (has_squawk_error)
			sctype = sqerror;

		// ----- Groundspeed -------
		string speed = std::to_string(reportedGs);
		for (size_t i = 0; i < 3 - speed.length(); i++)
			speed = "0" + speed;
		speed = speed.substr(0, 3);

		// ----- Departure runway -------
		string deprwy = fp.GetFlightPlanData().GetDepartureRwy();

		// ----- Departure runway that changes for overspeed -------
		string seprwy = fp.GetFlightPlanData().GetDepartureRwy();
		if (reportedGs > 25)
			seprwy = std::to_string(reportedGs);

		// ----- Gate -------
		string gate;
		if (CurrentConfig->getActiveProfile()["labels"]["use_aspeed_for_gate"].GetBool())
			gate = std::to_string(fp.GetControllerAssignedData().GetAssignedSpeed());
		else
			gate = fp.GetControllerAssignedData().GetScratchPadString();

		gate = gate.substr(0, 4);

		if (gate.size() == 0 || gate == "0")
			gate = "NOGT";

		// ----- Gate that changes to speed -------
		string sate = gate;
		if (reportedGs > 25)
			sate = speed; 

		// ----- Flightlevel -------
		string flightlevel = "F"; 
		int FL = RtPos.GetFlightLevel();
		if (RtPos.GetPressureAltitude() < GetPlugIn()->GetTransitionAltitude()) {
			flightlevel = "A";
			FL = RtPos.GetPressureAltitude();
		}
		string str = std::to_string(FL);
		for (size_t i = 0; i < 6 - str.length(); i++)
			str = "0" + str;
		str.erase(str.begin() + 3, str.end());
		flightlevel += str;

		// ----- Tendency -------
		string tendency = "";
		int delta_fl = FL - rt.GetPreviousPosition(RtPos).GetPressureAltitude();
		if (abs(delta_fl) >= 20) {
			if (delta_fl < 0) {
				tendency = "|";
			}
			else {
				tendency = "^";
			}
		}

		// ----- Wake cat -------
		string wake = "?";
		if (fp.IsValid()) {
			wake = "";
			wake += fp.GetFlightPlanData().GetAircraftWtc();
		}

		// ----- Generating the replacing map -----
		map<string, string> TagReplacingMap;
		TagReplacingMap["callsign"] = callsign;
		TagReplacingMap["actype"] = actype;
		TagReplacingMap["sctype"] = sctype;
		TagReplacingMap["sqerror"] = sqerror;
		TagReplacingMap["deprwy"] = deprwy;
		TagReplacingMap["seprwy"] = seprwy;
		TagReplacingMap["gate"] = gate;
		TagReplacingMap["sate"] = sate;
		TagReplacingMap["flightlevel"] = flightlevel;
		TagReplacingMap["gs"] = speed;
		TagReplacingMap["tendency"] = tendency;
		TagReplacingMap["wake"] = wake;

		// ----- Generating the clickable map -----
		map<string, int> TagClickableMap;
		TagClickableMap[callsign] = TAG_CITEM_CALLSIGN;
		TagClickableMap[actype] = TAG_CITEM_FPBOX;
		TagClickableMap[sctype] = TAG_CITEM_FPBOX;
		TagClickableMap[sqerror] = TAG_CITEM_NO;
		TagClickableMap[deprwy] = TAG_CITEM_RWY;
		TagClickableMap[seprwy] = TAG_CITEM_RWY;
		TagClickableMap[gate] = TAG_CITEM_GATE;
		TagClickableMap[sate] = TAG_CITEM_GATE;
		TagClickableMap[flightlevel] = TAG_CITEM_NO;
		TagClickableMap[speed] = TAG_CITEM_NO;
		TagClickableMap[tendency] = TAG_CITEM_NO;
		TagClickableMap[wake] = TAG_CITEM_FPBOX;

		//
		// ----- Now the hard part, drawing (using gdi+) -------
		//	

		struct Utils2 {
			static string getEnumString(TagTypes type) {
				if (type == TagTypes::Departure)
					return "departure";
				if (type == TagTypes::Arrival)
					return "arrival";
				if (type == TagTypes::Airborne)
					return "airborne";
			}
		};

		// First we need to figure out the tag size
		
		int TagWidth, TagHeight;
		const Value& LabelsSettings = CurrentConfig->getActiveProfile()["labels"];

		string line1_size = "";
		string line2_size = "";
		for (SizeType i = 0; i < LabelsSettings[Utils2::getEnumString(TagType).c_str()]["line1"].Size(); i++) {
			const Value& item = LabelsSettings[Utils2::getEnumString(TagType).c_str()]["line1"][i];
			line1_size.append(item.GetString());
			if (i != LabelsSettings[Utils2::getEnumString(TagType).c_str()]["line1"].Size() - 1)
				line1_size.append(" ");
		}

			// If there is a second line, then we determine it's size.
		if (LabelsSettings[Utils2::getEnumString(TagType).c_str()]["two_lines_tag"].GetBool()) {
			for (SizeType i = 0; i < LabelsSettings[Utils2::getEnumString(TagType).c_str()]["line2"].Size(); i++) {
				const Value& item = LabelsSettings[Utils2::getEnumString(TagType).c_str()]["line2"][i];
				line2_size.append(item.GetString());
				if (i != LabelsSettings[Utils2::getEnumString(TagType).c_str()]["line2"].Size() - 1)
					line2_size.append(" ");
			}
		}

		for (std::map<string, string>::iterator it = TagReplacingMap.begin(); it != TagReplacingMap.end(); ++it)
		{
			replaceAll(line1_size, it->first, it->second);

			if (LabelsSettings[Utils2::getEnumString(TagType).c_str()]["two_lines_tag"].GetBool()) {
				replaceAll(line2_size, it->first, it->second);
			}
		}

		if (CurrentConfig->getActiveProfile()["font"]["use_custom_font"].GetBool()) {
			wstring line1_sizew = wstring(line1_size.begin(), line1_size.end());
			wstring line2_sizew = wstring(line2_size.begin(), line2_size.end());

			RectF line1Box, line2Box;
			
			graphics.MeasureString(line1_sizew.c_str(), wcslen(line1_sizew.c_str()), customFonts[currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &line1Box);
			graphics.MeasureString(line2_sizew.c_str(), wcslen(line2_sizew.c_str()), customFonts[currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &line2Box);
			TagWidth = max(line1Box.GetRight(), line2Box.GetRight());
			TagHeight = (line1Box.GetBottom() + line2Box.GetBottom())-2;
		}
		else {
			CRect line1Size(0, 0, 0, 0);
			dc.DrawText(line1_size.c_str(), &line1Size, DT_CALCRECT);
			CRect line2Size(0, 0, 0, 0);
			dc.DrawText(line2_size.c_str(), &line2Size, DT_CALCRECT);

			TagWidth = max(line1Size.right, line2Size.right);
			TagHeight = line1Size.bottom+line2Size.bottom;
		}

		// Pfiou, done with that, now we can draw the actual rectangle.

		//COLORREF TagBackgroundColor2 = RimcasInstance->GetAircraftColor(rt.GetCallsign());
		Color TagBackgroundColor = CurrentConfig->getConfigColor(LabelsSettings[Utils2::getEnumString(TagType).c_str()]["background_color"]);
		
		CRect TagBackgroundRect(TagCenter.x - (TagWidth / 2), TagCenter.y - (TagHeight / 2), TagCenter.x + (TagWidth / 2), TagCenter.y + (TagHeight / 2));
		SolidBrush TagBackgroundBrush(TagBackgroundColor);
		graphics.FillRectangle(&TagBackgroundBrush, CopyRect(TagBackgroundRect));

		if (CurrentConfig->getActiveProfile()["font"]["use_custom_font"].GetBool()) {
			SolidBrush FontColor(CurrentConfig->getConfigColor(LabelsSettings[Utils2::getEnumString(TagType).c_str()]["text_color"]));
			wstring line1w = wstring(line1_size.begin(), line1_size.end());
			graphics.DrawString(line1w.c_str(), wcslen(line1w.c_str()), customFonts[currentFontSize], PointF(TagBackgroundRect.left, TagBackgroundRect.top), &Gdiplus::StringFormat(), &FontColor);
			
			if (LabelsSettings[Utils2::getEnumString(TagType).c_str()]["two_lines_tag"].GetBool()) {
				wstring line2w = wstring(line2_size.begin(), line2_size.end());
				graphics.DrawString(line2w.c_str(), wcslen(line2w.c_str()), customFonts[currentFontSize], PointF(TagBackgroundRect.left, TagBackgroundRect.top + (TagHeight / 2)), &Gdiplus::StringFormat(), &FontColor);
			}
		}
		else {
			COLORREF oldColor = dc.SetTextColor(CurrentConfig->getConfigColorRef(LabelsSettings[Utils2::getEnumString(TagType).c_str()]["text_color"]));

			dc.TextOutA(TagBackgroundRect.left, TagBackgroundRect.top, line1_size.c_str());
			if (LabelsSettings[Utils2::getEnumString(TagType).c_str()]["two_lines_tag"].GetBool()) {
				dc.TextOutA(TagBackgroundRect.left, TagBackgroundRect.top + (TagHeight / 2), line2_size.c_str());
			}

			dc.SetTextColor(oldColor);
		}
		
		// Drawing the leader line
		RECT TagBackRectData = TagBackgroundRect;
		POINT toDraw1, toDraw2;
		if (LiangBarsky(TagBackRectData, acPosPix, TagBackgroundRect.CenterPoint(), toDraw1, toDraw2))
			graphics.DrawLine(&Pen(Color::White), PointF(acPosPix.x, acPosPix.y), PointF(toDraw1.x, toDraw1.y));

		// Adding the tag screen object

		AddScreenObject(DRAWING_TAG, rt.GetCallsign(), TagBackgroundRect, true, GetBottomLine(rt.GetCallsign()).c_str());

		// Now adding the clickable zones

		// We need to get the size of a blank space
		int blank_space_width = 0;
		CRect blankSpace(0, 0, 0, 0);
		dc.DrawText(" ", &blankSpace, DT_CALCRECT);
		int blank_space_width = blankSpace.right;

		vector<string> line1_items = split(line1_size, ' ');
		int offset = 0;
		for (auto &item : line1_items)
		{
			if (TagClickableMap.find(item) != TagClickableMap.end()) {
				// We need to get the area that text covers

				if (TagClickableMap[item] == TAG_CITEM_NO)
					continue;

				int ItemWidth, ItemHeight;
				if (CurrentConfig->getActiveProfile()["font"]["use_custom_font"].GetBool()) {
					wstring item_sizew = wstring(item.begin(), item.end());

					RectF itemBox;

					graphics.MeasureString(item_sizew.c_str(), wcslen(item_sizew.c_str()), customFonts[currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &itemBox);
					ItemWidth = itemBox.GetRight();
					ItemHeight = itemBox.GetBottom() - 2;
				}
				else {
					CRect itemSize(0, 0, 0, 0);
					dc.DrawText(item.c_str(), &itemSize, DT_CALCRECT);

					ItemWidth = itemSize.right;
					ItemHeight = itemSize.bottom;
				}

				// We then calculate the rectangle
				CRect ItemRect(TagBackgroundRect.left + offset, 
					TagBackgroundRect.top, 
					(TagBackgroundRect.left + offset) + ItemWidth,
					TagBackgroundRect.top + ItemHeight);

				// We then add the screen object
				AddScreenObject(TagClickableMap[item], rt.GetCallsign(), ItemRect, false, GetBottomLine(rt.GetCallsign()).c_str());

				// Finally, we update the offset

				offset += ItemWidth + blank_space_width;
			}
		}

		// If there is a line 2, then we do it all over again :p
		if (LabelsSettings[Utils2::getEnumString(TagType).c_str()]["two_lines_tag"].GetBool()) {
			vector<string> line2_items = split(line2_size, ' ');
			offset = 0;
			for (auto &item : line1_items)
			{
				if (TagClickableMap.find(item) != TagClickableMap.end()) {
					// We need to get the area that text covers

					if (TagClickableMap[item] == TAG_CITEM_NO)
						continue;

					int ItemWidth, ItemHeight;
					if (CurrentConfig->getActiveProfile()["font"]["use_custom_font"].GetBool()) {
						wstring item_sizew = wstring(item.begin(), item.end());

						RectF itemBox;

						graphics.MeasureString(item_sizew.c_str(), wcslen(item_sizew.c_str()), customFonts[currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &itemBox);
						ItemWidth = itemBox.GetRight();
						ItemHeight = itemBox.GetBottom() - 2;
					}
					else {
						CRect itemSize(0, 0, 0, 0);
						dc.DrawText(item.c_str(), &itemSize, DT_CALCRECT);

						ItemWidth = itemSize.right;
						ItemHeight = itemSize.bottom;
					}

					// We then calculate the rectangle
					CRect ItemRect(TagBackgroundRect.left + offset,
						TagBackgroundRect.top + (TagHeight / 2),
						(TagBackgroundRect.left + offset) + ItemWidth,
						(TagBackgroundRect.top + (TagHeight / 2) ) + ItemHeight);

					// We then add the screen object
					AddScreenObject(TagClickableMap[item], rt.GetCallsign(), ItemRect, false, GetBottomLine(rt.GetCallsign()).c_str());

					// Finally, we update the offset

					offset += ItemWidth + blank_space_width;
				}
			}
		}
	}

	/*
	SolidBrush TestNight(CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["labels"]["airborne"]["background_color"]));
	CRect tN = new CRect(GetRadarArea());
	graphics.FillRectangle(&TestNight, CopyRect(tN));
	*/

#pragma endregion Drawing of the tags

	// Releasing the hDC after the drawing
	graphics.ReleaseHDC(hDC);

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

		if (!CurrentConfig->getActiveProfile()["rimcas"]["short_timer"].GetBool()) {
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

	if (ShowLists["Conflict Alert ARR"]) {
		GetPlugIn()->OpenPopupList(ListAreas["Conflict Alert ARR"], "CA Arrival", 1);
		for (std::map<string, CRimcas::RunwayAreaType>::iterator it = RimcasInstance->RunwayAreas.begin(); it != RimcasInstance->RunwayAreas.end(); ++it)
		{
			GetPlugIn()->AddPopupListElement(it->first.c_str(), "", RIMCAS_CA_ARRIVAL_FUNC, false, RimcasInstance->MonitoredRunwayArr[it->first.c_str()]);
		}
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		ShowLists["Conflict Alert ARR"] = false;
	}

	if (ShowLists["Conflict Alert DEP"]) {
		GetPlugIn()->OpenPopupList(ListAreas["Conflict Alert DEP"], "CA Departure", 1);
		for (std::map<string, CRimcas::RunwayAreaType>::iterator it = RimcasInstance->RunwayAreas.begin(); it != RimcasInstance->RunwayAreas.end(); ++it)
		{
			GetPlugIn()->AddPopupListElement(it->first.c_str(), "", RIMCAS_CA_MONITOR_FUNC, false, RimcasInstance->MonitoredRunwayDep[it->first.c_str()]);
		}
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		ShowLists["Conflict Alert DEP"] = false;
	}

	if (ShowLists["Runway closed"]) {
		GetPlugIn()->OpenPopupList(ListAreas["Runway closed"], "Runway Closed", 1);
		for (std::map<string, CRimcas::RunwayAreaType>::iterator it = RimcasInstance->RunwayAreas.begin(); it != RimcasInstance->RunwayAreas.end(); ++it)
		{
			GetPlugIn()->AddPopupListElement(it->first.c_str(), "", RIMCAS_CLOSED_RUNWAYS_FUNC, false, RimcasInstance->ClosedRunway[it->first.c_str()]);
		}
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		ShowLists["Runway closed"] = false;
	}

	if (ShowLists["Visibility"]) {
		GetPlugIn()->OpenPopupList(ListAreas["Visibility"], "Visibility", 1);
		GetPlugIn()->AddPopupListElement("Normal", "", RIMCAS_CLOSE);
		GetPlugIn()->AddPopupListElement("Low", "", RIMCAS_CLOSE);
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		ShowLists["Visibility"] = false;
	}

	//---------------------------------
	// QRD
	//---------------------------------

	if (QDMenabled) {
		CPen Pen(PS_SOLID, 1, RGB(255, 255, 255));
		CPen *oldPen = dc.SelectObject(&Pen);

		POINT AirportPos = ConvertCoordFromPositionToPixel(AirportPositions[getActiveAirport()]);
		dc.MoveTo(AirportPos);
		POINT p;
		if (GetCursorPos(&p))
		{
			dc.LineTo(p);

			CPosition CursorPos = ConvertCoordFromPixelToPosition(p);
			double Distance = AirportPositions[getActiveAirport()].DistanceTo(CursorPos);
			double Bearing = AirportPositions[getActiveAirport()].DirectionTo(CursorPos);

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
		RequestRefresh();
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
	dc.TextOutA(ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, getActiveAirport().c_str());
	AddScreenObject(7999, "ActiveAirport", { ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, ToolBarAreaTop.left + offset + dc.GetTextExtent(getActiveAirport().c_str()).cx, ToolBarAreaTop.top + 4 + dc.GetTextExtent(getActiveAirport().c_str()).cy }, false, "Active Airport");

	offset += dc.GetTextExtent(getActiveAirport().c_str()).cx + 10;
	dc.TextOutA(ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, "Display");
	AddScreenObject(8000, "DisplayMenu", { ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, ToolBarAreaTop.left + offset + dc.GetTextExtent("Display").cx, ToolBarAreaTop.top + 4 + dc.GetTextExtent("Display").cy }, false, "Display menu");

	offset += dc.GetTextExtent("Display").cx + 10;
	dc.TextOutA(ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, "Alerts");
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

			dist = AirportPositions[getActiveAirport()].DistanceTo(pt1);
			dir = TrueBearing(AirportPositions[getActiveAirport()], pt1);

			t1s.x = refPt.x + int(scale * dist * sin(dir) + 0.5);
			t1s.y = refPt.y - int(scale * dist * cos(dir) + 0.5);

			dist = AirportPositions[getActiveAirport()].DistanceTo(pt2);
			dir = TrueBearing(AirportPositions[getActiveAirport()], pt2);

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
				dist = AirportPositions[getActiveAirport()].DistanceTo(it->second.threshold);
				dir = TrueBearing(AirportPositions[getActiveAirport()], it->second.threshold);

				t1s.x = refPt.x + int(scale * dist * sin(dir) + 0.5);
				t1s.y = refPt.y - int(scale * dist * cos(dir) + 0.5);
				CPen PenC(PS_SOLID, 1, RGB(255, 255, 255));
				dc.SelectObject(PenC);

				double revHdg = it->second.bearing;
				double exsize = 18520.0;
				CPosition Far = Haversine(it->second.threshold, RadToDeg(float(revHdg)), exsize);
				POINT pt;
				revHdg = TrueBearing(AirportPositions[getActiveAirport()], Far);
				exsize = AirportPositions[getActiveAirport()].DistanceTo(Far);
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
					revHdg = TrueBearing(AirportPositions[getActiveAirport()], Far2);
					exsize = AirportPositions[getActiveAirport()].DistanceTo(Far2);
					pt2.x = refPt.x + int(scale * exsize * sin(revHdg) + 0.5);
					pt2.y = refPt.y - int(scale * exsize * cos(revHdg) + 0.5);

					revHdg = TrueBearing(AirportPositions[getActiveAirport()], Far3);
					exsize = AirportPositions[getActiveAirport()].DistanceTo(Far3);
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

			hdg = TrueBearing(AirportPositions[getActiveAirport()], RtPos2);
			dist = AirportPositions[getActiveAirport()].DistanceTo(RtPos2);
			RtPoint.x = refPt.x + int(scale * dist * sin(hdg) + 0.5);
			RtPoint.y = refPt.y - int(scale * dist * cos(hdg) + 0.5);

			CRadarTargetPositionData hPos = rt.GetPreviousPosition(rt.GetPosition());
			for (int i = 1; i < 4; i++) {
				if (!hPos.IsValid())
					continue;

				hdg = TrueBearing(AirportPositions[getActiveAirport()], hPos.GetPosition());
				dist = AirportPositions[getActiveAirport()].DistanceTo(hPos.GetPosition());
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
