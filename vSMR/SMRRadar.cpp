#include "stdafx.h"
#include "SMRRadar.hpp"

RECT TimePopupArea = { 300, 300, 430, 363 };

ULONG_PTR m_gdiplusToken;
int currentFontSize = 1;
map<int, Gdiplus::Font *> customFonts;
CPoint mouseLocation(0, 0);
string TagBeingDragged;
int LeaderLineDefaultlenght = 50;

bool mouseWithin(CRect rect) {
	if (mouseLocation.x >= rect.left + 1 && mouseLocation.x <= rect.right - 1 && mouseLocation.y >= rect.top + 1 && mouseLocation.y <= rect.bottom - 1)
		return true;
	return false;
}
inline std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
};
inline std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
};
inline float closest(std::vector<float> const& vec, float value) {
	auto const it = std::lower_bound(vec.begin(), vec.end(), value);
	if (it == vec.end()) { return -1; }

	return *it;
};
inline bool IsTagBeingDragged(string c)
{
	return TagBeingDragged == c;
}

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

	appWindowIds[1] = APPWINDOW_ONE;
	appWindowIds[2] = APPWINDOW_TWO;
	appWindowDisplays[1] = false;
	appWindowDisplays[2] = false;
	appWindowAreas[1] = { 200, 200, 600, 500 };
	appWindowAreas[2] = { 200, 200, 600, 500 };
	appWindowOffsets[1] = { 0, 0 };
	appWindowOffsets[2] = { 0, 0 };
	appWindowScales[1] = 15;
	appWindowScales[2] = 15;
	appWindowFilters[1] = 6000;
	appWindowFilters[2] = 6000;
	appWindowOffsetsGrip[1] = false;
	appWindowOffsetsGrip[2] = false;
	appWindowOffsetsInit[1] = { 0, 0 };
	appWindowOffsetsInit[2] = { 0, 0 };
	appWindowOffsetsDrag[1] = { 0, 0 };
	appWindowOffsetsDrag[2] = { 0, 0 };
	

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
	const Value& FSizes = CurrentConfig->getActiveProfile()["font"]["sizes"];
	string font_name = CurrentConfig->getActiveProfile()["font"]["font_name"].GetString();
	wstring buffer = wstring(font_name.begin(), font_name.end());
	Gdiplus::FontStyle fontStyle = Gdiplus::FontStyleRegular;
	if (strcmp(CurrentConfig->getActiveProfile()["font"]["weight"].GetString(), "Bold") == 0)
		fontStyle = Gdiplus::FontStyleBold;
	if (strcmp(CurrentConfig->getActiveProfile()["font"]["weight"].GetString(), "Italic") == 0)
		fontStyle = Gdiplus::FontStyleItalic;

	customFonts[1] = new Gdiplus::Font(buffer.c_str(), Gdiplus::REAL(FSizes["one"].GetInt()), fontStyle, Gdiplus::UnitPixel);
	customFonts[2] = new Gdiplus::Font(buffer.c_str(), Gdiplus::REAL(FSizes["two"].GetInt()), fontStyle, Gdiplus::UnitPixel);
	customFonts[3] = new Gdiplus::Font(buffer.c_str(), Gdiplus::REAL(FSizes["three"].GetInt()), fontStyle, Gdiplus::UnitPixel);
	customFonts[4] = new Gdiplus::Font(buffer.c_str(), Gdiplus::REAL(FSizes["four"].GetInt()), fontStyle, Gdiplus::UnitPixel);
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

	if ((p_value = GetDataFromAsr("Profile")) != NULL)
		CurrentConfig->setActiveProfile(string(p_value));

	/*
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
	}*/

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

	SaveDataToAsr("Profile", "vSMR active profile", CurrentConfig->getActiveProfileName().c_str());

	/*const char * to_save = "0";
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
	SaveDataToAsr("ShortTimer", "Timer lenght", to_save);*/

}

void CSMRRadar::OnMoveScreenObject(int ObjectType, const char * sObjectId, POINT Pt, RECT Area, bool Released) {

	if (ObjectType == APPWINDOW_ONE || APPWINDOW_TWO) {
		int appWindowId = ObjectType - APPWINDOW_BASE;

		if (strcmp(sObjectId, "window") == 0) {
			if (!appWindowOffsetsGrip[appWindowId])
			{
				appWindowOffsetsInit[appWindowId] = appWindowOffsets[appWindowId];
				appWindowOffsetsDrag[appWindowId] = Pt;
				appWindowOffsetsGrip[appWindowId] = true;
			}
			POINT maxoffset = { (appWindowAreas[appWindowId].right - appWindowAreas[appWindowId].left) / 2, 
				(appWindowAreas[appWindowId].bottom - (appWindowAreas[appWindowId].top + 15)) / 2 };
			appWindowOffsets[appWindowId].x = max(-maxoffset.x, min(maxoffset.x, appWindowOffsetsInit[appWindowId].x + (Pt.x - appWindowOffsetsDrag[appWindowId].x)));
			appWindowOffsets[appWindowId].y = max(-maxoffset.y, min(maxoffset.y, appWindowOffsetsInit[appWindowId].y + (Pt.y - appWindowOffsetsDrag[appWindowId].y)));

			if (Released)
			{
				appWindowOffsetsGrip[appWindowId] = false;
			}
		}
		if (strcmp(sObjectId, "resize") == 0) {
			POINT TopLeft = { appWindowAreas[appWindowId].left, appWindowAreas[appWindowId].top };
			POINT BottomRight = { Area.right, Area.bottom };
			CRect newSize(TopLeft, BottomRight);
			newSize.NormalizeRect();

			if (newSize.Height() < 200) {
				newSize.top = appWindowAreas[appWindowId].top;
				newSize.bottom = appWindowAreas[appWindowId].bottom;
			}
				
			if (newSize.Width() < 400) {
				newSize.left = appWindowAreas[appWindowId].left;
				newSize.right = appWindowAreas[appWindowId].right;
			}

			appWindowAreas[appWindowId] = newSize;
		}
		if (strcmp(sObjectId, "topbar") == 0) {
			CRect appWindowRect(appWindowAreas[appWindowId]);
			appWindowRect.NormalizeRect();

			POINT TopLeft = { Area.left, Area.bottom + 1 };
			POINT BottomRight = { TopLeft.x + appWindowRect.Width(), TopLeft.y + appWindowRect.Height() };
			CRect newPos(TopLeft, BottomRight);
			newPos.NormalizeRect();

			appWindowAreas[appWindowId] = newPos;
		}
	}

	if (ObjectType == DRAWING_TAG || ObjectType == TAG_CITEM_CALLSIGN || ObjectType == TAG_CITEM_FPBOX || ObjectType == TAG_CITEM_RWY || ObjectType == TAG_CITEM_GATE) {
		CRadarTarget rt = GetPlugIn()->RadarTargetSelect(sObjectId);

		if (!Released)
		{
			AfxGetApp()->DoWaitCursor(1);
		} else
		{
			AfxGetApp()->DoWaitCursor(-1);
		}

		if (rt.IsValid()) {
			CRect Temp = Area;
			POINT TagCenterPix = Temp.CenterPoint();
			POINT AcPosPix = ConvertCoordFromPositionToPixel(GetPlugIn()->RadarTargetSelect(sObjectId).GetPosition().GetPosition());
			POINT CustomTag = { TagCenterPix.x - AcPosPix.x, TagCenterPix.y - AcPosPix.y };
			//TagsOffsets[sObjectId] = CustomTag;
			float angle = RadToDeg(atan2(CustomTag.y, CustomTag.x));
			angle = fmod(angle + 360, 360);
			vector<float> angles;
			for (float k = 0.0f; k <= 360.0f; k += 22.5f)
				angles.push_back(k);

			TagAngles[sObjectId] = closest(angles, angle);
			TagLeaderLineLength[sObjectId] = max(LeaderLineDefaultlenght, min(DistancePts(AcPosPix, TagCenterPix), LeaderLineDefaultlenght*2));

			GetPlugIn()->SetASELAircraft(GetPlugIn()->FlightPlanSelect(sObjectId));

			if (Released)
			{
				TagBeingDragged = "";
			} else
			{
				TagBeingDragged = sObjectId;
			}

			RequestRefresh();
		}
	}

	if (ObjectType == 7000) {
		TimePopupAreas[sObjectId] = Area;
		RequestRefresh();
	}

	mouseLocation = Pt;
	RequestRefresh();

}

void CSMRRadar::OnOverScreenObject(int ObjectType, const char * sObjectId, POINT Pt, RECT Area)
{
	/*
	if (ObjectType == DRAWING_AC_SYMBOL)
	{
		CRadarTarget rt = GetPlugIn()->RadarTargetSelect(sObjectId);

		if (rt.IsValid())
		{
			OverAcSymbol[sObjectId] = clock();
			RequestRefresh();
		}
	}*/

	mouseLocation = Pt;
	RequestRefresh();
}

void CSMRRadar::OnClickScreenObject(int ObjectType, const char * sObjectId, POINT Pt, RECT Area, int Button)
{
	mouseLocation = Pt;

	if (ObjectType == APPWINDOW_ONE || APPWINDOW_TWO) {
		int appWindowId = ObjectType - APPWINDOW_BASE;

		if (strcmp(sObjectId, "close") == 0)
			appWindowDisplays[appWindowId] = false;
		if (strcmp(sObjectId, "range") == 0) {
			GetPlugIn()->OpenPopupEdit(Area, RIMCAS_UPDATERANGE + appWindowId, std::to_string(appWindowScales[appWindowId]).c_str());
		}
		if (strcmp(sObjectId, "filter") == 0) {
			GetPlugIn()->OpenPopupList(Area, "Filter", 1);
			GetPlugIn()->AddPopupListElement("66000", "", RIMCAS_UPDATEFILTER + appWindowId);
			GetPlugIn()->AddPopupListElement("24500", "", RIMCAS_UPDATEFILTER + appWindowId);
			GetPlugIn()->AddPopupListElement("8000", "", RIMCAS_UPDATEFILTER + appWindowId);
			GetPlugIn()->AddPopupListElement("6000", "", RIMCAS_UPDATEFILTER + appWindowId);
			GetPlugIn()->AddPopupListElement("3000", "", RIMCAS_UPDATEFILTER + appWindowId);
			GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		}
	}

	if (ObjectType == RIMCAS_ACTIVE_AIRPORT) {
		GetPlugIn()->OpenPopupEdit(Area, RIMCAS_ACTIVE_AIRPORT_FUNC, getActiveAirport().c_str());
	}

	if (ObjectType == DRAWING_BACKGROUND_CLICK)
	{
		if (QDMSelectEnabled)
		{
			if (Button == BUTTON_LEFT)
			{
				QDMSelectPt = Pt;
				RequestRefresh();
			}
		}
	}

	if (ObjectType == RIMCAS_MENU) {

		if (strcmp(sObjectId, "DisplayMenu") == 0) {
			Area.top = Area.top + 30;
			Area.bottom = Area.bottom + 30;

			GetPlugIn()->OpenPopupList(Area, "Display Menu", 1);
			GetPlugIn()->AddPopupListElement("QDR Fixed Reference", "", RIMCAS_QDM_TOGGLE);
			GetPlugIn()->AddPopupListElement("QDR Select Reference", "", RIMCAS_QDM_SELECT_TOGGLE);
			GetPlugIn()->AddPopupListElement("Approach Inset 1", "", APPWINDOW_ONE);
			GetPlugIn()->AddPopupListElement("Approach Inset 2", "", APPWINDOW_TWO);
			GetPlugIn()->AddPopupListElement("Font size", "", RIMCAS_OPEN_LIST);
			GetPlugIn()->AddPopupListElement("Profiles", "", RIMCAS_OPEN_LIST);
			GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		}

		if (strcmp(sObjectId, "MapMenu") == 0) {
			Area.top = Area.top + 30;
			Area.bottom = Area.bottom + 30;

			GetPlugIn()->OpenPopupList(Area, "Maps", 1);
			GetPlugIn()->AddPopupListElement("Airport Maps", "", RIMCAS_OPEN_LIST);
			GetPlugIn()->AddPopupListElement("Custom Maps", "", RIMCAS_OPEN_LIST);
			GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		}

		if (strcmp(sObjectId, "ColourMenu") == 0) {
			Area.top = Area.top + 30;
			Area.bottom = Area.bottom + 30;

			GetPlugIn()->OpenPopupList(Area, "Colours", 1);
			GetPlugIn()->AddPopupListElement("Colour Settings", "", RIMCAS_OPEN_LIST);
			GetPlugIn()->AddPopupListElement("Brightness", "", RIMCAS_OPEN_LIST);
			GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		}

		if (strcmp(sObjectId, "RIMCASMenu") == 0) {
			Area.top = Area.top + 30;
			Area.bottom = Area.bottom + 30;

			GetPlugIn()->OpenPopupList(Area, "Alerts", 1);
			GetPlugIn()->AddPopupListElement("Conflict Alert ARR", "", RIMCAS_OPEN_LIST);
			GetPlugIn()->AddPopupListElement("Conflict Alert DEP", "", RIMCAS_OPEN_LIST);
			GetPlugIn()->AddPopupListElement("Runway closed", "", RIMCAS_OPEN_LIST);
			GetPlugIn()->AddPopupListElement("Visibility", "", RIMCAS_OPEN_LIST);
			GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		}
		
	}

	if (ObjectType == DRAWING_TAG || ObjectType == DRAWING_AC_SYMBOL) {
		CFlightPlan Fp = GetPlugIn()->FlightPlanSelect(sObjectId);
		CRadarTarget rt = GetPlugIn()->RadarTargetSelect(sObjectId);
		if (rt.GetCorrelatedFlightPlan().IsValid()) {
			StartTagFunction(rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_TYPE_CALLSIGN, rt.GetCallsign(), NULL, EuroScopePlugIn::TAG_ITEM_FUNCTION_NO, Pt, Area);
		}
		else {
			GetPlugIn()->SetASELAircraft(Fp);
		}

		if (ObjectType == DRAWING_AC_SYMBOL)
		{
			if (QDMSelectEnabled)
			{
				if (Button == BUTTON_LEFT)
				{
					QDMSelectPt = Pt;
					RequestRefresh();
				}
			} else
			{
				if (TagsOffsets.find(sObjectId) != TagsOffsets.end())
					TagsOffsets.erase(sObjectId);

				if (Button == BUTTON_LEFT)
				{
					if (TagAngles.find(sObjectId) == TagAngles.end())
					{
						TagAngles[sObjectId] = 0;
					} else
					{
						TagAngles[sObjectId] = fmod(TagAngles[sObjectId] - 22.5f, 360);
					}
				}

				if (Button == BUTTON_RIGHT)
				{
					if (TagAngles.find(sObjectId) == TagAngles.end())
					{
						TagAngles[sObjectId] = 0;
					}
					else
					{
						TagAngles[sObjectId] = fmod(TagAngles[sObjectId] + 22.5f, 360);
					}
				}

				RequestRefresh();
			}
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
	mouseLocation = Pt;
	if (FunctionId == APPWINDOW_ONE || APPWINDOW_TWO) {
		int id = FunctionId - APPWINDOW_BASE;
		appWindowDisplays[id] = !appWindowDisplays[id];
	}

	if (FunctionId == RIMCAS_ACTIVE_AIRPORT_FUNC) {
		setActiveAirport(sItemString);
		SaveDataToAsr("Airport", "Active airport", getActiveAirport().c_str());
	}

	if (FunctionId == RIMCAS_UPDATE_FONTS) {
		if (strcmp(sItemString, "Size 1") == 0)
			currentFontSize = 1;
		if (strcmp(sItemString, "Size 2") == 0)
			currentFontSize = 2;
		if (strcmp(sItemString, "Size 3") == 0)
			currentFontSize = 3;
		if (strcmp(sItemString, "Size 4") == 0)
			currentFontSize = 4;

		ShowLists["Font size"] = true;
	}

	if (FunctionId == RIMCAS_QDM_TOGGLE) {
		QDMenabled = !QDMenabled;
		QDMSelectEnabled = false;
	}

	if (FunctionId == RIMCAS_QDM_SELECT_TOGGLE)
	{
		if (!QDMSelectEnabled)
		{
			QDMSelectPt = ConvertCoordFromPositionToPixel(AirportPositions[getActiveAirport()]);
		}
		QDMSelectEnabled = !QDMSelectEnabled;
		QDMenabled = false;
	}

	if (FunctionId == RIMCAS_UPDATE_PROFILE) {
		CurrentConfig->setActiveProfile(sItemString);
		LoadCustomFont();
		SaveDataToAsr("Profile", "vSMR active profile", sItemString);

		ShowLists["Profiles"] = true;
	}

	if (FunctionId == RIMCAS_UPDATEFILTER1 || FunctionId == RIMCAS_UPDATEFILTER2) {
		int id = FunctionId - RIMCAS_UPDATEFILTER;
		appWindowFilters[id] = atoi(sItemString);
	}

	if (FunctionId == RIMCAS_UPDATERANGE1 || FunctionId == RIMCAS_UPDATERANGE2) {
		int id = FunctionId - RIMCAS_UPDATERANGE;
		appWindowScales[id] = atoi(sItemString);
	}

	if (FunctionId == RIMCAS_UPDATE_BRIGHNESS) {
		if (strcmp(sItemString, "Day") == 0)
			ColorSettingsDay = true;
		else
			ColorSettingsDay = false;

		ShowLists["Colour Settings"] = true;

		RequestRefresh();
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

		ShowLists["Runway closed"] = true;

		RequestRefresh();
	}

	if (FunctionId == RIMCAS_OPEN_LIST) {

		ShowLists[string(sItemString)] = true;
		ListAreas[string(sItemString)] = Area;

		RequestRefresh();
	}

	if (FunctionId == RIMCAS_UPDATE_LVP) {
		if (strcmp(sItemString, "Normal") == 0)
			isLVP = false;
		if (strcmp(sItemString, "Low") == 0)
			isLVP = true;

		ShowLists["Visibility"] = true;

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


	width = width + float((rand() % 5) - 2);
	cabin_width = cabin_width + float((rand() % 3) - 1);
	lenght = lenght + float((rand() % 5) - 2);


	float trackHead = float(RadarTarget.GetPosition().GetReportedHeadingTrueNorth());
	float inverseTrackHead = float(fmod(trackHead + 180.0f, 360));
	float leftTrackHead = float(fmod(trackHead - 90.0f, 360));
	float rightTrackHead = float(fmod(trackHead + 90.0f, 360));

	float HalfLenght = lenght / 2.0f;
	float HalfCabWidth = cabin_width / 2.0f;
	float HalfSpanWidth = width / 2.0f;

	// Base shape is like a deformed cross


	CPosition topMiddle = Haversine(RtPos.GetPosition(), trackHead, HalfLenght);
	CPosition topLeft = Haversine(topMiddle, leftTrackHead, HalfCabWidth);
	CPosition topRight = Haversine(topMiddle, rightTrackHead, HalfCabWidth);

	CPosition bottomMiddle = Haversine(RtPos.GetPosition(), inverseTrackHead, HalfLenght);
	CPosition bottomLeft = Haversine(bottomMiddle, leftTrackHead, HalfCabWidth);
	CPosition bottomRight = Haversine(bottomMiddle, rightTrackHead, HalfCabWidth);

	CPosition middleTopLeft = Haversine(topLeft, float(fmod(inverseTrackHead + 25.0f, 360)), 0.8f*HalfLenght);
	CPosition middleTopRight = Haversine(topRight, float(fmod(inverseTrackHead - 25.0f, 360)), 0.8f*HalfLenght);
	CPosition middleBottomLeft = Haversine(bottomLeft, float(fmod(trackHead - 15.0f, 360)), 0.8f*HalfLenght);
	CPosition middleBottomRight = Haversine(bottomRight, float(fmod(trackHead + 15.0f, 360)), 0.8f*HalfLenght);

	CPosition rightTop = Haversine(middleBottomRight, rightTrackHead, 0.7f*HalfSpanWidth);
	CPosition rightBottom = Haversine(rightTop, inverseTrackHead, cabin_width);

	CPosition leftTop = Haversine(middleBottomLeft, leftTrackHead, 0.7f*HalfSpanWidth);
	CPosition leftBottom = Haversine(leftTop, inverseTrackHead, cabin_width);

	CPosition basePoints[12];
	basePoints[0] = topLeft;
	basePoints[1] = middleTopLeft;
	basePoints[2] = leftTop;
	basePoints[3] = leftBottom;
	basePoints[4] = middleBottomLeft;
	basePoints[5] = bottomLeft;
	basePoints[6] = bottomRight;
	basePoints[7] = middleBottomRight;
	basePoints[8] = rightBottom;
	basePoints[9] = rightTop;
	basePoints[10] = middleTopRight;
	basePoints[11] = topRight;

	// 12 points total, so 11 from 0
	// ------

	// Random points between points of base shape

	for (int i = 0; i < 12; i++){

		CPosition newPoint, lastPoint, endPoint, startPoint;

		startPoint = basePoints[i];
		if (i == 11) endPoint = basePoints[0];
		else endPoint = basePoints[i + 1];

		double dist, rndHeading;
		dist = startPoint.DistanceTo(endPoint);

		Patatoides[RadarTarget.GetCallsign()].points[i * 7] = { startPoint.m_Latitude, startPoint.m_Longitude };
		lastPoint = startPoint;

		for (int k = 1; k < 7; k++){

			rndHeading = float(fmod(lastPoint.DirectionTo(endPoint) + (-25.0 + (rand() % 50 + 1)), 360));
			newPoint = Haversine(lastPoint, rndHeading, dist * 200);
			Patatoides[RadarTarget.GetCallsign()].points[(i * 7) + k] = { newPoint.m_Latitude, newPoint.m_Longitude };
			lastPoint = newPoint;
		}
	}
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
			string rfl_s;
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
	double p = 0, q = 0, r;

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
	if (Phase == REFRESH_PHASE_AFTER_LISTS) {
		if (!ColorSettingsDay) {
			// Creating the gdi+ graphics
			Graphics graphics(hDC);
			graphics.SetPageUnit(Gdiplus::UnitPixel);

			graphics.SetSmoothingMode(SmoothingModeAntiAlias);

			SolidBrush AlphaBrush(Color(CurrentConfig->getActiveProfile()["filters"]["night_alpha_setting"].GetInt(), 0, 0, 0));

			CRect RadarArea(GetRadarArea());
			RadarArea.bottom = GetChatArea().bottom;

			graphics.FillRectangle(&AlphaBrush, CopyRect(CRect(RadarArea)));

			graphics.ReleaseHDC(hDC);
		}
	}

	if (Phase != REFRESH_PHASE_BEFORE_TAGS)
		return;

	struct Utils {
		static RECT GetAreaFromText(CDC * dc, string text, POINT Pos) {
			RECT Area = { Pos.x, Pos.y, Pos.x + dc->GetTextExtent(text.c_str()).cx, Pos.y + dc->GetTextExtent(text.c_str()).cy };
			return Area;
		}
	};


	POINT p;
	if (GetCursorPos(&p))
	{
		mouseLocation = p;
	}

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

	// Creating the gdi+ graphics
	Graphics graphics(hDC);
	graphics.SetPageUnit(Gdiplus::UnitPixel);

	graphics.SetSmoothingMode(SmoothingModeAntiAlias);

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

	if (QDMSelectEnabled)
	{
		CRect R(GetRadarArea());
		R.top += 20;
		R.bottom = GetChatArea().top;

		R.NormalizeRect();
		AddScreenObject(DRAWING_BACKGROUND_CLICK, "", R, false, "");
	}

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

			const Value& CustomMap = CurrentConfig->getAirportMapIfAny(getActiveAirport());

			if (CurrentConfig->isCustomRunwayAvail(getActiveAirport(), runway_name, runway_name2)) {
				const Value& Runways = CustomMap["runways"];

				assert(Runways.IsArray());

				vector<CPosition> def;

				for (SizeType i = 0; i < Runways.Size(); i++) {
					if (startsWith(runway_name.c_str(), Runways[i]["runway_name"].GetString()) || 
						startsWith(runway_name2.c_str(), Runways[i]["runway_name"].GetString())) {
						
						string path_name = "path";

						if (isLVP)
							path_name = "path_lvp";

						const Value& Path = Runways[i][path_name.c_str()];
						for (SizeType j = 0; j < Path.Size(); j++) {
							CPosition p;
							p.LoadFromStrings(Path[j][(SizeType)1].GetString(), Path[j][(SizeType)0].GetString());

							def.push_back(p);
						}
						
					}
				}

				RimcasInstance->AddCustomRunway(runway_name, runway_name2, Left, Right, def);
			}
			else {
				RimcasInstance->AddRunwayArea(this, runway_name, runway_name2, Left, Right, bearing1, bearing2);
			}

			if (RimcasInstance->ClosedRunway.find(runway_name) != RimcasInstance->ClosedRunway.end() || RimcasInstance->ClosedRunway.find(runway_name2) != RimcasInstance->ClosedRunway.end()) {
				if (RimcasInstance->ClosedRunway[runway_name] || RimcasInstance->ClosedRunway[runway_name2]) {

					CPen RedPen(PS_SOLID, 2, RGB(150, 0, 0));
					CPen * oldPen = dc.SelectObject(&RedPen);

					if (CurrentConfig->isCustomRunwayAvail(getActiveAirport(), runway_name, runway_name2)) {
						const Value& Runways = CustomMap["runways"];
						
						assert(Runways.IsArray());

						for (SizeType i = 0; i < Runways.Size(); i++) {
							if (startsWith(runway_name.c_str(), Runways[i]["runway_name"].GetString()) || 
								startsWith(runway_name2.c_str(), Runways[i]["runway_name"].GetString())) {

								string path_name = "path";

								if (isLVP)
									path_name = "path_lvp";

								const Value& Path = Runways[i][path_name.c_str()];

								PointF lpPoints[5000];

								int k = 1;
								int l = 0;
								for (SizeType w = 0; w < Path.Size(); w++) {
									CPosition p;
									p.LoadFromStrings(Path[w][static_cast<SizeType>(1)].GetString(), Path[w][static_cast<SizeType>(0)].GetString());

									POINT cv = ConvertCoordFromPositionToPixel(p);
									lpPoints[l] = { REAL(cv.x), REAL(cv.y) };

									k++;
									l++;
								}
								
								graphics.FillPolygon(&SolidBrush(Color(150, 0, 0)), lpPoints, k-1);

								break;
							}
						}

					}
					else {
						CRimcas::RunwayAreaType Area = RimcasInstance->GetRunwayArea(this, Left, Right, 0, bearing1);

						POINT TopLeftPt = ConvertCoordFromPositionToPixel(Area.topLeft);

						POINT BottomRightPt = ConvertCoordFromPositionToPixel(Area.bottomRight);

						POINT TopRightPt = ConvertCoordFromPositionToPixel(Area.topRight);

						POINT BottomLeftPt = ConvertCoordFromPositionToPixel(Area.bottomLeft);

						dc.MoveTo(TopLeftPt);
						dc.LineTo(TopRightPt);

						dc.MoveTo(TopRightPt);
						dc.LineTo(BottomRightPt);

						dc.MoveTo(BottomRightPt);
						dc.LineTo(BottomLeftPt);

						dc.MoveTo(BottomLeftPt);
						dc.LineTo(TopLeftPt);
					}

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

		RimcasInstance->OnRefresh(rt, this, IsCorrelated(GetPlugIn()->FlightPlanSelect(rt.GetCallsign()), rt));

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
					SolidBrush H_Brush(CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["targets"]["history_one_color"]));
					
					PointF lpPoints[100];
					for (unsigned int i = 0; i < Patatoides[rt.GetCallsign()].History_one_points.size(); i++)
					{
						CPosition pos;
						pos.m_Latitude = Patatoides[rt.GetCallsign()].History_one_points[i].x;
						pos.m_Longitude = Patatoides[rt.GetCallsign()].History_one_points[i].y;

						lpPoints[i] = { REAL(ConvertCoordFromPositionToPixel(pos).x), REAL(ConvertCoordFromPositionToPixel(pos).y) };
					}
					graphics.FillPolygon(&H_Brush, lpPoints, Patatoides[rt.GetCallsign()].History_one_points.size());
				}

				if (i != 2) {
					if (!Patatoides[rt.GetCallsign()].History_two_points.empty() && CurrentConfig->getActiveProfile()["targets"]["show_primary_target"].GetBool()) {
						SolidBrush H_Brush(CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["targets"]["history_two_color"]));

						PointF lpPoints[100];
						for (unsigned int i = 0; i < Patatoides[rt.GetCallsign()].History_two_points.size(); i++)
						{
							CPosition pos;
							pos.m_Latitude = Patatoides[rt.GetCallsign()].History_two_points[i].x;
							pos.m_Longitude = Patatoides[rt.GetCallsign()].History_two_points[i].y;

							lpPoints[i] = { REAL(ConvertCoordFromPositionToPixel(pos).x), REAL(ConvertCoordFromPositionToPixel(pos).y) };
						}
						graphics.FillPolygon(&H_Brush, lpPoints, Patatoides[rt.GetCallsign()].History_two_points.size());
					}
				}

				if (i == 2 && !Patatoides[rt.GetCallsign()].History_three_points.empty() && CurrentConfig->getActiveProfile()["targets"]["show_primary_target"].GetBool()) {
					SolidBrush H_Brush(CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["targets"]["history_three_color"]));

					PointF lpPoints[100];
					for (unsigned int i = 0; i < Patatoides[rt.GetCallsign()].History_three_points.size(); i++)
					{
						CPosition pos;
						pos.m_Latitude = Patatoides[rt.GetCallsign()].History_three_points[i].x;
						pos.m_Longitude = Patatoides[rt.GetCallsign()].History_three_points[i].y;

						lpPoints[i] = { REAL(ConvertCoordFromPositionToPixel(pos).x), REAL(ConvertCoordFromPositionToPixel(pos).y) };
					}
					graphics.FillPolygon(&H_Brush, lpPoints, Patatoides[rt.GetCallsign()].History_three_points.size());
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

			SolidBrush H_Brush(CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["targets"]["target_color"]));

			PointF lpPoints[100];
			for (unsigned int i = 0; i < Patatoides[rt.GetCallsign()].points.size(); i++)
			{
				CPosition pos;
				pos.m_Latitude = Patatoides[rt.GetCallsign()].points[i].x;
				pos.m_Longitude = Patatoides[rt.GetCallsign()].points[i].y;

				lpPoints[i] = { REAL(ConvertCoordFromPositionToPixel(pos).x), REAL(ConvertCoordFromPositionToPixel(pos).y) };
			}

			graphics.FillPolygon(&H_Brush, lpPoints, Patatoides[rt.GetCallsign()].points.size());
		}
		acPosPix = ConvertCoordFromPositionToPixel(RtPos.GetPosition());

		bool AcisCorrelated = IsCorrelated(GetPlugIn()->FlightPlanSelect(rt.GetCallsign()), rt);

		if (!AcisCorrelated && reportedGs < 3)
			continue;

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

		if (mouseWithin({ acPosPix.x - 5, acPosPix.y - 5, acPosPix.x + 5, acPosPix.y + 5 })) {
			dc.MoveTo(acPosPix.x, acPosPix.y - 8);
			dc.LineTo(acPosPix.x - 6, acPosPix.y - 12);
			dc.MoveTo(acPosPix.x, acPosPix.y - 8);
			dc.LineTo(acPosPix.x + 6, acPosPix.y - 12);

			dc.MoveTo(acPosPix.x, acPosPix.y + 8);
			dc.LineTo(acPosPix.x - 6, acPosPix.y + 12);
			dc.MoveTo(acPosPix.x, acPosPix.y + 8);
			dc.LineTo(acPosPix.x + 6, acPosPix.y + 12);

			dc.MoveTo(acPosPix.x - 8, acPosPix.y );
			dc.LineTo(acPosPix.x - 12, acPosPix.y -6);
			dc.MoveTo(acPosPix.x - 8, acPosPix.y);
			dc.LineTo(acPosPix.x - 12 , acPosPix.y + 6);

			dc.MoveTo(acPosPix.x + 8, acPosPix.y);
			dc.LineTo(acPosPix.x + 12, acPosPix.y - 6);
			dc.MoveTo(acPosPix.x + 8, acPosPix.y);
			dc.LineTo(acPosPix.x + 12, acPosPix.y + 6);
		}

		AddScreenObject(DRAWING_AC_SYMBOL, rt.GetCallsign(), { acPosPix.x - 5, acPosPix.y - 5, acPosPix.x + 5, acPosPix.y + 5 }, false, AcisCorrelated ? GetBottomLine(rt.GetCallsign()).c_str() : rt.GetSystemID());

		dc.SelectObject(pqOrigPen);
	}

#pragma endregion Drawing of the symbols

	TimePopupData.clear();
	AcOnRunway.clear();
	ColorAC.clear();
	tagAreas.clear();

	RimcasInstance->OnRefreshEnd(this, CurrentConfig->getActiveProfile()["rimcas"]["rimcas_stage_two_speed_threshold"].GetInt());

	graphics.SetSmoothingMode(SmoothingModeDefault);

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

		bool AcisCorrelated = IsCorrelated(fp, rt);

		if (!AcisCorrelated && reportedGs < 3)
			isAcDisplayed = false;

		if (!isAcDisplayed)
			continue;

		// Getting the tag center/offset

		POINT TagCenter;
		map<string, POINT>::iterator it = TagsOffsets.find(rt.GetCallsign());
		if (it != TagsOffsets.end()) {
			TagCenter = { acPosPix.x + it->second.x, acPosPix.y + it->second.y };
		}
		else {
			// Use angle:

			if (TagAngles.find(rt.GetCallsign()) == TagAngles.end())
				TagAngles[rt.GetCallsign()] = 270.0f;

			int lenght = LeaderLineDefaultlenght;
			if (TagLeaderLineLength.find(rt.GetCallsign()) != TagLeaderLineLength.end())
				lenght = TagLeaderLineLength[rt.GetCallsign()];

			TagCenter.x = long(acPosPix.x + float(lenght * cos(DegToRad(TagAngles[rt.GetCallsign()]))));
			TagCenter.y = long(acPosPix.y + float(lenght * sin(DegToRad(TagAngles[rt.GetCallsign()]))));
		}

		TagTypes TagType = TagTypes::Departure;

		if (fp.IsValid() && strcmp(fp.GetFlightPlanData().GetDestination(), getActiveAirport().c_str()) == 0) {
			TagType = TagTypes::Arrival;
		}

		if (reportedGs > 50) {
			TagType = TagTypes::Airborne;
		}

		if (!AcisCorrelated && reportedGs >= 3)
		{
			TagType = TagTypes::Uncorrelated;
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
		// ssr: the current squawk of the ac
		// dep: the assigned SID
		// sdep: a short version of the SID
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
		if (deprwy.length() == 0)
			deprwy = "RWY";

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
		string tendency = "-";
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

		// ----- SSR -------
		string tssr = "";
		if (rt.IsValid())
		{
			tssr = rt.GetPosition().GetSquawk();
		}

		// ----- DEP -------
		string dep = "";
		if (fp.IsValid())
		{
			dep = fp.GetFlightPlanData().GetSidName();
		}

		// ----- Short DEP -------
		string sdep = dep;
		if (fp.IsValid() && sdep.size() > 5)
		{
			sdep = dep.substr(0, 3);
			sdep += dep.substr(dep.size() - 2, dep.size());
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
		TagReplacingMap["ssr"] = tssr;
		TagReplacingMap["adep"] = dep;
		TagReplacingMap["sdep"] = sdep;

		// System ID for uncorrelated
		TagReplacingMap["systemid"] = "T:";
		string tpss = rt.GetSystemID();
		TagReplacingMap["systemid"].append(tpss.substr(2, 6));

		// ----- Generating the clickable map -----
		map<string, int> TagClickableMap;
		TagClickableMap[callsign] = TAG_CITEM_CALLSIGN;
		TagClickableMap[actype] = TAG_CITEM_FPBOX;
		TagClickableMap[sctype] = TAG_CITEM_FPBOX;
		TagClickableMap[sqerror] = TAG_CITEM_FPBOX;
		TagClickableMap[deprwy] = TAG_CITEM_RWY;
		TagClickableMap[seprwy] = TAG_CITEM_RWY;
		TagClickableMap[gate] = TAG_CITEM_GATE;
		TagClickableMap[sate] = TAG_CITEM_GATE;
		TagClickableMap[flightlevel] = TAG_CITEM_NO;
		TagClickableMap[speed] = TAG_CITEM_NO;
		TagClickableMap[tendency] = TAG_CITEM_NO;
		TagClickableMap[wake] = TAG_CITEM_FPBOX; 
		TagClickableMap[tssr] = TAG_CITEM_NO;
		TagClickableMap[dep] = TagClickableMap[sdep] = TAG_CITEM_NO;

		//
		// ----- Now the hard part, drawing (using gdi+) -------
		//	

		struct Utils2 {
			static string getEnumString(TagTypes type) {
				if (type == TagTypes::Departure)
					return "departure";
				if (type == TagTypes::Arrival)
					return "arrival";
				if (type == TagTypes::Uncorrelated)
					return "uncorrelated";
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

		// If there is a second line, then we determine its size.
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

		wstring line1_sizew = wstring(line1_size.begin(), line1_size.end());
		wstring line2_sizew = wstring(line2_size.begin(), line2_size.end());

		RectF line1Box, line2Box;
			
		graphics.MeasureString(line1_sizew.c_str(), wcslen(line1_sizew.c_str()), customFonts[currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &line1Box);
		graphics.MeasureString(line2_sizew.c_str(), wcslen(line2_sizew.c_str()), customFonts[currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &line2Box);
		TagWidth = int(max(line1Box.GetRight(), line2Box.GetRight()));
		TagHeight = int((line1Box.GetBottom() + line2Box.GetBottom())-2);

		// Pfiou, done with that, now we can draw the actual rectangle.

		// We need to figure out if the tag color changes according to RIMCAS alerts, or not
		bool rimcasLabelOnly = CurrentConfig->getActiveProfile()["rimcas"]["rimcas_label_only"].GetBool();

		Color TagBackgroundColor = RimcasInstance->GetAircraftColor(rt.GetCallsign(), 
			CurrentConfig->getConfigColor(LabelsSettings[Utils2::getEnumString(TagType).c_str()]["background_color"]),
			CurrentConfig->getConfigColor(LabelsSettings[Utils2::getEnumString(TagType).c_str()]["background_color_on_runway"]),
			CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["rimcas"]["background_color_stage_one"]),
			CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["rimcas"]["background_color_stage_two"]));

		if (rimcasLabelOnly)
			TagBackgroundColor = RimcasInstance->GetAircraftColor(rt.GetCallsign(),
			CurrentConfig->getConfigColor(LabelsSettings[Utils2::getEnumString(TagType).c_str()]["background_color"]),
			CurrentConfig->getConfigColor(LabelsSettings[Utils2::getEnumString(TagType).c_str()]["background_color_on_runway"]));
	
		CRect TagBackgroundRect(TagCenter.x - (TagWidth / 2), TagCenter.y - (TagHeight / 2), TagCenter.x + (TagWidth / 2), TagCenter.y + (TagHeight / 2));
		SolidBrush TagBackgroundBrush(TagBackgroundColor);
		graphics.FillRectangle(&TagBackgroundBrush, CopyRect(TagBackgroundRect));
		if (mouseWithin(TagBackgroundRect) || IsTagBeingDragged(rt.GetCallsign()))
		{
			Pen pw(Color::White);
			graphics.DrawRectangle(&pw, CopyRect(TagBackgroundRect));
		}

		SolidBrush FontColor(CurrentConfig->getConfigColor(LabelsSettings[Utils2::getEnumString(TagType).c_str()]["text_color"]));
		wstring line1w = wstring(line1_size.begin(), line1_size.end());
		graphics.DrawString(line1w.c_str(), wcslen(line1w.c_str()), customFonts[currentFontSize], PointF(Gdiplus::REAL(TagBackgroundRect.left), Gdiplus::REAL(TagBackgroundRect.top)), &Gdiplus::StringFormat(), &FontColor);
			
		if (LabelsSettings[Utils2::getEnumString(TagType).c_str()]["two_lines_tag"].GetBool()) {
			wstring line2w = wstring(line2_size.begin(), line2_size.end());
			graphics.DrawString(line2w.c_str(), wcslen(line2w.c_str()), customFonts[currentFontSize], PointF(Gdiplus::REAL(TagBackgroundRect.left), Gdiplus::REAL(TagBackgroundRect.top + (TagHeight / 2))), &Gdiplus::StringFormat(), &FontColor);
		}
		
		// Drawing the leader line
		RECT TagBackRectData = TagBackgroundRect;
		POINT toDraw1, toDraw2;
		if (LiangBarsky(TagBackRectData, acPosPix, TagBackgroundRect.CenterPoint(), toDraw1, toDraw2))
			graphics.DrawLine(&Pen(Color::White), PointF(Gdiplus::REAL(acPosPix.x), Gdiplus::REAL(acPosPix.y)), PointF(Gdiplus::REAL(toDraw1.x), Gdiplus::REAL(toDraw1.y)));

		// If we use a RIMCAS label only, we display it, and adapt the rectangle
		CRect oldCrectSave = TagBackgroundRect;

		if (rimcasLabelOnly) {
			Color RimcasLabelColor = RimcasInstance->GetAircraftColor(rt.GetCallsign(), Color::AliceBlue, Color::AliceBlue,
				CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["rimcas"]["background_color_stage_one"]),
				CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["rimcas"]["background_color_stage_two"]));

			if (RimcasLabelColor.ToCOLORREF() != Color(Color::AliceBlue).ToCOLORREF()) {
				int rimcas_height = 0;

				wstring wrimcas_height = wstring(L"ALERT");

				RectF RectRimcas_height;

				graphics.MeasureString(wrimcas_height.c_str(), wcslen(wrimcas_height.c_str()), customFonts[currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &RectRimcas_height);
				rimcas_height = int(RectRimcas_height.GetBottom());

				// Drawing the rectangle

				CRect RimcasLabelRect(TagBackgroundRect.left, TagBackgroundRect.top - rimcas_height, TagBackgroundRect.right, TagBackgroundRect.top);
				graphics.FillRectangle(&SolidBrush(RimcasLabelColor), CopyRect(RimcasLabelRect));
				TagBackgroundRect.top -= rimcas_height;

				// Drawing the text
				
				SolidBrush FontColor(Color::White);
				wstring rimcasw = wstring(L"ALERT");
				StringFormat stformat = new StringFormat();
				stformat.SetAlignment(StringAlignment::StringAlignmentCenter);
				graphics.DrawString(rimcasw.c_str(), wcslen(rimcasw.c_str()), customFonts[currentFontSize], PointF(Gdiplus::REAL((TagBackgroundRect.left + TagBackgroundRect.right) / 2), Gdiplus::REAL(TagBackgroundRect.top)), &stformat, &FontColor);

			}
		}

		// Adding the tag screen object
		tagAreas[rt.GetCallsign()] = TagBackgroundRect;
		AddScreenObject(DRAWING_TAG, rt.GetCallsign(), TagBackgroundRect, true, GetBottomLine(rt.GetCallsign()).c_str());

		TagBackgroundRect = oldCrectSave;

		// Now adding the clickable zones

		// We need to get the size of a blank space
		int blank_space_width = 0;
		CRect blankSpace(0, 0, 0, 0);
		dc.DrawText(" ", &blankSpace, DT_CALCRECT);
		blank_space_width = blankSpace.right;

		vector<string> line1_items = split(line1_size, ' ');
		int offset = 0;
		for (auto &item : line1_items)
		{
			if (TagClickableMap.find(item) != TagClickableMap.end()) {
				// We need to get the area that text covers

				if (TagClickableMap[item] == TAG_CITEM_NO)
					continue;

				int ItemWidth, ItemHeight;
				wstring item_sizew = wstring(item.begin(), item.end());

				RectF itemBox;

				graphics.MeasureString(item_sizew.c_str(), wcslen(item_sizew.c_str()), customFonts[currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &itemBox);
				ItemWidth = int(itemBox.GetRight());
				ItemHeight = int(itemBox.GetBottom() - 2);

				// We then calculate the rectangle
				CRect ItemRect(TagBackgroundRect.left + offset, 
					TagBackgroundRect.top, 
					(TagBackgroundRect.left + offset) + ItemWidth,
					TagBackgroundRect.top + ItemHeight);

				// If there is a squawk error and the item is a squawk error, we re-draw it with the color

				if (has_squawk_error && strcmp(item.c_str(), sqerror.c_str()) == 0) {
					SolidBrush FontColor(CurrentConfig->getConfigColor(LabelsSettings["squawk_error_color"]));
					wstring sqw = wstring(sqerror.begin(), sqerror.end());
					graphics.DrawString(sqw.c_str(), wcslen(sqw.c_str()), customFonts[currentFontSize], PointF(Gdiplus::REAL(ItemRect.left), Gdiplus::REAL(ItemRect.top)), &Gdiplus::StringFormat(), &FontColor);
				}

				// We then add the screen object
				AddScreenObject(TagClickableMap[item], rt.GetCallsign(), ItemRect, true, GetBottomLine(rt.GetCallsign()).c_str());

				// Finally, we update the offset

				offset += ItemWidth + blank_space_width;
			}
		}

		// If there is a line 2, then we do it all over again :p
		if (LabelsSettings[Utils2::getEnumString(TagType).c_str()]["two_lines_tag"].GetBool()) {
			vector<string> line2_items = split(line2_size, ' ');
			offset = 0;
			for (auto &item : line2_items)
			{
				if (TagClickableMap.find(item) != TagClickableMap.end()) {
					// We need to get the area that text covers

					if (TagClickableMap[item] == TAG_CITEM_NO)
						continue;

					int ItemWidth, ItemHeight;
					wstring item_sizew = wstring(item.begin(), item.end());

					RectF itemBox;

					graphics.MeasureString(item_sizew.c_str(), wcslen(item_sizew.c_str()), customFonts[currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &itemBox);
					ItemWidth = int(itemBox.GetRight());
					ItemHeight = int(itemBox.GetBottom() - 2);

					// We then calculate the rectangle
					CRect ItemRect(TagBackgroundRect.left + offset,
						TagBackgroundRect.top + (TagHeight / 2),
						(TagBackgroundRect.left + offset) + ItemWidth,
						(TagBackgroundRect.top + (TagHeight / 2) ) + ItemHeight);

					// If there is a squawk error and the item is a squawk error, we re-draw it with the color

					if (has_squawk_error && strcmp(item.c_str(), sqerror.c_str()) == 0) {
							SolidBrush FontColor(CurrentConfig->getConfigColor(LabelsSettings["squawk_error_color"]));
							wstring sqw = wstring(sqerror.begin(), sqerror.end());
							graphics.DrawString(sqw.c_str(), wcslen(sqw.c_str()), customFonts[currentFontSize], PointF(Gdiplus::REAL(ItemRect.left), Gdiplus::REAL(ItemRect.top)), &Gdiplus::StringFormat(), &FontColor);
					}

					// We then add the screen object
					AddScreenObject(TagClickableMap[item], rt.GetCallsign(), ItemRect, true, GetBottomLine(rt.GetCallsign()).c_str());

					// Finally, we update the offset

					offset += ItemWidth + blank_space_width;
				}
			}
		}
	}

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
			CBrush OrangeBrush(RimcasInstance->GetAircraftColor(RimcasInstance->TimeTable[it->first][30], 
				Color::Black, 
				Color::Black,
				CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["rimcas"]["background_color_stage_one"]),
				CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["rimcas"]["background_color_stage_two"])).ToCOLORREF()
				); // RGB(180, 100, 50)
			CRect TempRect = { CRectTime.left, CRectTime.top + TextHeight * 3, CRectTime.right, CRectTime.top + TextHeight * 4 };
			dc.FillRect(TempRect, &OrangeBrush);
			dc.SetTextColor(RGB(238, 238, 208));
		}
		dc.TextOutA(CRectTime.left, CRectTime.top + TextHeight * 3, tempS.c_str());
		tempS = std::to_string(times[4]) + ": " + RimcasInstance->TimeTable[it->first][times[4]];
		dc.SetTextColor(RGB(33, 33, 33));
		if (RimcasInstance->AcColor.find(RimcasInstance->TimeTable[it->first][times[4]]) != RimcasInstance->AcColor.end()) {
			CBrush OrangeBrush(RimcasInstance->GetAircraftColor(RimcasInstance->TimeTable[it->first][times[4]],
				Color::Black,
				Color::Black,
				CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["rimcas"]["background_color_stage_one"]),
				CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["rimcas"]["background_color_stage_two"])).ToCOLORREF()
				); // RGB(180, 100, 50)
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
		GetPlugIn()->AddPopupListElement("Normal", "", RIMCAS_UPDATE_LVP, false, int(!isLVP));
		GetPlugIn()->AddPopupListElement("Low", "", RIMCAS_UPDATE_LVP, false, int(isLVP));
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		ShowLists["Visibility"] = false;
	}

	if (ShowLists["Profiles"]) {
		GetPlugIn()->OpenPopupList(ListAreas["Profiles"], "Profiles", 1);
		vector<string> allProfiles = CurrentConfig->getAllProfiles();
		for (std::vector<string>::iterator it = allProfiles.begin(); it != allProfiles.end(); ++it) {
			GetPlugIn()->AddPopupListElement(it->c_str(), "", RIMCAS_UPDATE_PROFILE, false, int(CurrentConfig->isItActiveProfile(it->c_str())));
		}
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		ShowLists["Profiles"] = false;
	}
	
	if (ShowLists["Colour Settings"]) {
		GetPlugIn()->OpenPopupList(ListAreas["Colour Settings"], "Colour Settings", 1);
		GetPlugIn()->AddPopupListElement("Day", "", RIMCAS_UPDATE_BRIGHNESS, false, int(ColorSettingsDay));
		GetPlugIn()->AddPopupListElement("Night", "", RIMCAS_UPDATE_BRIGHNESS, false, int(!ColorSettingsDay));
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		ShowLists["Colour Settings"] = false;
	}

	if (ShowLists["Font size"]) {
		GetPlugIn()->OpenPopupList(ListAreas["Font size"], "Font size", 1);
		GetPlugIn()->AddPopupListElement("Size 1", "", RIMCAS_UPDATE_FONTS, false, int(bool(currentFontSize == 1)));
		GetPlugIn()->AddPopupListElement("Size 2", "", RIMCAS_UPDATE_FONTS, false, int(bool(currentFontSize == 2)));
		GetPlugIn()->AddPopupListElement("Size 3", "", RIMCAS_UPDATE_FONTS, false, int(bool(currentFontSize == 3)));
		GetPlugIn()->AddPopupListElement("Size 4", "", RIMCAS_UPDATE_FONTS, false, int(bool(currentFontSize == 4)));
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		ShowLists["Font size"] = false;
	}
	

	//---------------------------------
	// QRD
	//---------------------------------

	if (QDMenabled || QDMSelectEnabled) {
		CPen Pen(PS_SOLID, 1, RGB(255, 255, 255));
		CPen *oldPen = dc.SelectObject(&Pen);

		POINT AirportPos = ConvertCoordFromPositionToPixel(AirportPositions[getActiveAirport()]);
		CPosition AirportCPos = AirportPositions[getActiveAirport()];
		if (QDMSelectEnabled)
		{
			AirportPos = QDMSelectPt;
			AirportCPos = ConvertCoordFromPixelToPosition(QDMSelectPt);
		}
		dc.MoveTo(AirportPos);
		POINT p;
		if (GetCursorPos(&p))
		{
			dc.LineTo(p);

			CPosition CursorPos = ConvertCoordFromPixelToPosition(p);
			double Distance = AirportCPos.DistanceTo(CursorPos);
			double Bearing = AirportCPos.DirectionTo(CursorPos);

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

			string text = bearings;
			text += "° / ";
			text += distances;
			text += "m";
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
	AddScreenObject(RIMCAS_ACTIVE_AIRPORT, "ActiveAirport", { ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, ToolBarAreaTop.left + offset + dc.GetTextExtent(getActiveAirport().c_str()).cx, ToolBarAreaTop.top + 4 + dc.GetTextExtent(getActiveAirport().c_str()).cy }, false, "Active Airport");

	offset += dc.GetTextExtent(getActiveAirport().c_str()).cx + 10;
	dc.TextOutA(ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, "Display");
	AddScreenObject(RIMCAS_MENU, "DisplayMenu", { ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, ToolBarAreaTop.left + offset + dc.GetTextExtent("Display").cx, ToolBarAreaTop.top + 4 + dc.GetTextExtent("Display").cy }, false, "Display menu");

	offset += dc.GetTextExtent("Display").cx + 10;
	dc.TextOutA(ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, "Map");
	AddScreenObject(RIMCAS_MENU, "MapMenu", { ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, ToolBarAreaTop.left + offset + dc.GetTextExtent("Map").cx, ToolBarAreaTop.top + 4 + dc.GetTextExtent("Map").cy }, false, "Map menu");

	offset += dc.GetTextExtent("Map").cx + 10;
	dc.TextOutA(ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, "Colours");
	AddScreenObject(RIMCAS_MENU, "ColourMenu", { ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, ToolBarAreaTop.left + offset + dc.GetTextExtent("Colour").cx, ToolBarAreaTop.top + 4 + dc.GetTextExtent("Colour").cy }, false, "Colour menu");

	offset += dc.GetTextExtent("Colours").cx + 10;
	dc.TextOutA(ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, "Alerts");
	AddScreenObject(RIMCAS_MENU, "RIMCASMenu", { ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, ToolBarAreaTop.left + offset + dc.GetTextExtent("RIMCAS").cx, ToolBarAreaTop.top + 4 + +dc.GetTextExtent("RIMCAS").cy }, false, "RIMCAS menu");

	dc.SetTextColor(oldTextColor);

	//
	// Tag deconflicting
	//

	for (const auto areas : tagAreas)
	{
		if (TagsOffsets.find(areas.first) != TagsOffsets.end())
			continue;

		if (IsTagBeingDragged(areas.first))
			continue;

		if (RecentlyAutoMovedTags.find(areas.first) != RecentlyAutoMovedTags.end())
		{
			double delta_t = (double)clock() - RecentlyAutoMovedTags[areas.first] / ((double)CLOCKS_PER_SEC);
			if (delta_t >= 0.8) {
				RecentlyAutoMovedTags.erase(areas.first);
			} else
			{
				continue;
			}
		}

		// We need to see wether the rotation will be clockwise or anti-clockwise

		bool isAntiClockwise = false;

		for (const auto area2 : tagAreas)
		{
			if (areas.first == area2.first)
				continue;

			CRect h;

			if (h.IntersectRect(tagAreas[areas.first], area2.second))
			{
				if (areas.second.left <= area2.second.left)
				{
					isAntiClockwise = true;
				}
				
				break;
			}
		}

		// We then rotate the tags until we did a 360 or there is no more conflicts

		POINT acPosPix = ConvertCoordFromPositionToPixel(GetPlugIn()->RadarTargetSelect(areas.first.c_str()).GetPosition().GetPosition());
		int lenght = LeaderLineDefaultlenght;
		if (TagLeaderLineLength.find(areas.first) != TagLeaderLineLength.end())
			lenght = TagLeaderLineLength[areas.first];

		int width = areas.second.Width();
		int height = areas.second.Height();

		for (float rotated = 0.0f; abs(rotated) <= 360.0f;)
		{
			// We first rotate the tag
			float newangle = fmod(TagAngles[areas.first] + rotated, 360.0f);

			POINT TagCenter;
			TagCenter.x = long(acPosPix.x + float(lenght * cos(DegToRad(newangle))));
			TagCenter.y = long(acPosPix.y + float(lenght * sin(DegToRad(newangle))));

			CRect NewRectangle(TagCenter.x - (width / 2), TagCenter.y - (height / 2), TagCenter.x + (width / 2), TagCenter.y + (height / 2));
			NewRectangle.NormalizeRect();

			// Assume there is no conflict, then try again

			bool isTagConflicing = false;

			for (const auto area2 : tagAreas)
			{
				if (areas.first == area2.first)
					continue;

				CRect h;

				if (h.IntersectRect(NewRectangle, area2.second))
				{
					isTagConflicing = true;
					break;
				}
			}

			if (!isTagConflicing)
			{
				TagAngles[areas.first] = fmod(TagAngles[areas.first] + rotated, 360);
				tagAreas[areas.first] = NewRectangle;
				RecentlyAutoMovedTags[areas.first] = clock();
				break;
			}

			if (isAntiClockwise)
				rotated -= 22.5f;
			else
				rotated += 22.5f;
		}
	}

	//
	// App windows
	//

	for (std::map<int, bool>::iterator it = appWindowDisplays.begin(); it != appWindowDisplays.end(); ++it)
	{
		if (!it->second)
			continue;

		int appWindowId = it->first;

		COLORREF qBackgroundColor = CurrentConfig->getConfigColorRef(CurrentConfig->getActiveProfile()["approach_insets"]["background_color"]);
		CRect windowAreaCRect(appWindowAreas[appWindowId]);
		windowAreaCRect.NormalizeRect();
		dc.FillSolidRect(windowAreaCRect, qBackgroundColor);
		AddScreenObject(appWindowId + APPWINDOW_BASE, "window", appWindowAreas[appWindowId], true, "");

		int scale = appWindowScales[appWindowId]; // 10pix for 1nm

		POINT refPt = windowAreaCRect.CenterPoint();
		refPt.x += appWindowOffsets[appWindowId].x;
		refPt.y += appWindowOffsets[appWindowId].y;

		for (std::map<string, CRimcas::RunwayAreaType>::iterator it = RimcasInstance->RunwayAreas.begin(); it != RimcasInstance->RunwayAreas.end(); ++it)
		{
			CPosition pt1, pt2;
			pt1 = it->second.threshold;
			pt2 = it->second.threshold2;
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

			CPen PenT(PS_SOLID, 1, CurrentConfig->getConfigColorRef(CurrentConfig->getActiveProfile()["approach_insets"]["runway_color"]));
			dc.SelectObject(PenT);
			POINT toDraw1, toDraw2;
			if (LiangBarsky(appWindowAreas[appWindowId], t1s, t2s, toDraw1, toDraw2)) {
				dc.MoveTo(toDraw1);
				dc.LineTo(toDraw2);
			}

			/*// Extended centerlines
			if (RimcasInstance->MonitoredRunwayArr[it->first]) {
				dist = AirportPositions[getActiveAirport()].DistanceTo(it->second.threshold);
				dir = TrueBearing(AirportPositions[getActiveAirport()], it->second.threshold);

				t1s.x = refPt.x + int(scale * dist * sin(dir) + 0.5);
				t1s.y = refPt.y - int(scale * dist * cos(dir) + 0.5);
				CPen PenC(PS_SOLID, 1, CurrentConfig->getConfigColorRef(CurrentConfig->getActiveProfile()["approach_insets"]["extended_lines_color"]));
				dc.SelectObject(PenC);

				double revHdg = it->second.bearing;
				double exsize = double(CurrentConfig->getActiveProfile()["approach_insets"]["extended_lines_length"].GetInt()) * 1852.0;
				CPosition Far = Haversine(it->second.threshold, RadToDeg(float(revHdg)), exsize);
				POINT pt;
				revHdg = TrueBearing(AirportPositions[getActiveAirport()], Far);
				exsize = AirportPositions[getActiveAirport()].DistanceTo(Far);
				pt.x = refPt.x + int(scale * exsize * sin(revHdg) + 0.5);
				pt.y = refPt.y - int(scale * exsize * cos(revHdg) + 0.5);

				if (LiangBarsky(appWindowAreas[appWindowId], t1s, pt, toDraw1, toDraw2)) {
					dc.MoveTo(toDraw1);
					dc.LineTo(toDraw2);
				}

				int increment = CurrentConfig->getActiveProfile()["approach_insets"]["extended_lines_ticks_spacing"].GetInt() * 1852;

				for (int j = increment; j <= int(CurrentConfig->getActiveProfile()["approach_insets"]["extended_lines_length"].GetInt() * 1852); j += increment) {

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

					if (LiangBarsky(appWindowAreas[appWindowId], pt2, pt3, toDraw1, toDraw2)) {
						dc.MoveTo(toDraw1);
						dc.LineTo(toDraw2);
					}

				}

			} */

		}

		// Aircrafts

		CRect windowAreaRect2(appWindowAreas[appWindowId]);

		vector<POINT> appAreaVect = { windowAreaRect2.TopLeft(), { windowAreaRect2.right, windowAreaRect2.top }, windowAreaRect2.BottomRight(), { windowAreaRect2.left, windowAreaRect2.bottom } };
		CPen WhitePen(PS_SOLID, 1, RGB(255, 255, 255));

		for (rt = GetPlugIn()->RadarTargetSelectFirst();
			rt.IsValid();
			rt = GetPlugIn()->RadarTargetSelectNext(rt))
		{
			int radarRange = CurrentConfig->getActiveProfile()["filters"]["radar_range_nm"].GetInt();

			if (rt.GetGS() < 50 ||
				rt.GetPosition().GetPressureAltitude() > appWindowFilters[appWindowId] ||
				!rt.IsValid() ||
				!rt.GetPosition().IsValid() ||
				rt.GetPosition().GetPosition().DistanceTo(AirportPositions[getActiveAirport()]) > radarRange)
				continue;

			CPosition RtPos2 = rt.GetPosition().GetPosition();
			CRadarTargetPositionData RtPos = rt.GetPosition();
			POINT acPosPix = ConvertCoordFromPositionToPixel(RtPos.GetPosition());
			CFlightPlan fp = GetPlugIn()->FlightPlanSelect(rt.GetCallsign());
			int reportedGs = RtPos.GetReportedGS();

			// Filtering the targets

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

			int lenght = 35;

			POINT TagCenter;
			TagCenter.x = long(RtPoint.x + float(lenght * cos(DegToRad(45))));
			TagCenter.y = long(RtPoint.y + float(lenght * sin(DegToRad(45))));

			// Drawing the tags, what a fucking mess

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
			if (deprwy.length() == 0)
				deprwy = "RWY";

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
			string tendency = "-";
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

			// ----- SSR -------
			string tssr = "";
			if (rt.IsValid())
			{
				tssr = rt.GetPosition().GetSquawk();
			}

			// ----- DEP -------
			string dep = "";
			if (fp.IsValid())
			{
				dep = fp.GetFlightPlanData().GetSidName();
			}

			// ----- Short DEP -------
			string sdep = dep;
			if (fp.IsValid() && sdep.size() > 5)
			{
				sdep = dep.substr(0, 3);
				sdep += dep.substr(dep.size() - 2, dep.size());
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
			TagReplacingMap["srr"] = tssr;
			TagReplacingMap["adep"] = dep;
			TagReplacingMap["sdep"] = sdep;

			// ----- Generating the clickable map -----
			map<string, int> TagClickableMap;
			TagClickableMap[callsign] = TAG_CITEM_CALLSIGN;
			TagClickableMap[actype] = TAG_CITEM_FPBOX;
			TagClickableMap[sctype] = TAG_CITEM_FPBOX;
			TagClickableMap[sqerror] = TAG_CITEM_FPBOX;
			TagClickableMap[deprwy] = TAG_CITEM_RWY;
			TagClickableMap[seprwy] = TAG_CITEM_RWY;
			TagClickableMap[gate] = TAG_CITEM_GATE;
			TagClickableMap[sate] = TAG_CITEM_GATE;
			TagClickableMap[flightlevel] = TAG_CITEM_NO;
			TagClickableMap[speed] = TAG_CITEM_NO;
			TagClickableMap[tendency] = TAG_CITEM_NO;
			TagClickableMap[wake] = TAG_CITEM_FPBOX;
			TagClickableMap[tssr] = TAG_CITEM_NO;

			//
			// ----- Now the hard part, drawing (using gdi+) -------
			//	

			// First we need to figure out the tag size

			int TagWidth, TagHeight;
			const Value& LabelsSettings = CurrentConfig->getActiveProfile()["labels"];

			string line1_size = "";
			string line2_size = "";
			for (SizeType i = 0; i < LabelsSettings["airborne"]["line1"].Size(); i++) {
				const Value& item = LabelsSettings["airborne"]["line1"][i];
				line1_size.append(item.GetString());
				if (i != LabelsSettings["airborne"]["line1"].Size() - 1)
					line1_size.append(" ");
			}

			// If there is a second line, then we determine its size.
			if (LabelsSettings["airborne"]["two_lines_tag"].GetBool()) {
				for (SizeType i = 0; i < LabelsSettings["airborne"]["line2"].Size(); i++) {
					const Value& item = LabelsSettings["airborne"]["line2"][i];
					line2_size.append(item.GetString());
					if (i != LabelsSettings["airborne"]["line2"].Size() - 1)
						line2_size.append(" ");
				}
			}

			for (std::map<string, string>::iterator it = TagReplacingMap.begin(); it != TagReplacingMap.end(); ++it)
			{
				replaceAll(line1_size, it->first, it->second);

				if (LabelsSettings["airborne"]["two_lines_tag"].GetBool()) {
					replaceAll(line2_size, it->first, it->second);
				}
			}

			wstring line1_sizew = wstring(line1_size.begin(), line1_size.end());
			wstring line2_sizew = wstring(line2_size.begin(), line2_size.end());

			RectF line1Box, line2Box;

			graphics.MeasureString(line1_sizew.c_str(), wcslen(line1_sizew.c_str()), customFonts[currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &line1Box);
			graphics.MeasureString(line2_sizew.c_str(), wcslen(line2_sizew.c_str()), customFonts[currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &line2Box);
			TagWidth = int(max(line1Box.GetRight(), line2Box.GetRight()));
			TagHeight = int((line1Box.GetBottom() + line2Box.GetBottom()) - 2);

			// Pfiou, done with that, now we can draw the actual rectangle.

			// We need to figure out if the tag color changes according to RIMCAS alerts, or not
			bool rimcasLabelOnly = CurrentConfig->getActiveProfile()["rimcas"]["rimcas_label_only"].GetBool();

			Color TagBackgroundColor = RimcasInstance->GetAircraftColor(rt.GetCallsign(),
				CurrentConfig->getConfigColor(LabelsSettings["airborne"]["background_color"]),
				CurrentConfig->getConfigColor(LabelsSettings["airborne"]["background_color_on_runway"]),
				CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["rimcas"]["background_color_stage_one"]),
				CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["rimcas"]["background_color_stage_two"]));

			if (rimcasLabelOnly)
				TagBackgroundColor = RimcasInstance->GetAircraftColor(rt.GetCallsign(),
				CurrentConfig->getConfigColor(LabelsSettings["airborne"]["background_color"]),
				CurrentConfig->getConfigColor(LabelsSettings["airborne"]["background_color_on_runway"]));

			CRect TagBackgroundRect(TagCenter.x - (TagWidth / 2), TagCenter.y - (TagHeight / 2), TagCenter.x + (TagWidth / 2), TagCenter.y + (TagHeight / 2));

			if (Is_Inside(TagBackgroundRect.TopLeft(), appAreaVect) &&
				Is_Inside(RtPoint, appAreaVect) &&
				Is_Inside(TagBackgroundRect.BottomRight(), appAreaVect)) {

				SolidBrush TagBackgroundBrush(TagBackgroundColor);
				graphics.FillRectangle(&TagBackgroundBrush, CopyRect(TagBackgroundRect));

				SolidBrush FontColor(CurrentConfig->getConfigColor(LabelsSettings["airborne"]["text_color"]));
				wstring line1w = wstring(line1_size.begin(), line1_size.end());
				graphics.DrawString(line1w.c_str(), wcslen(line1w.c_str()), customFonts[currentFontSize], PointF(Gdiplus::REAL(TagBackgroundRect.left), Gdiplus::REAL(TagBackgroundRect.top)), &Gdiplus::StringFormat(), &FontColor);

				if (LabelsSettings["airborne"]["two_lines_tag"].GetBool()) {
					wstring line2w = wstring(line2_size.begin(), line2_size.end());
					graphics.DrawString(line2w.c_str(), wcslen(line2w.c_str()), customFonts[currentFontSize], PointF(Gdiplus::REAL(TagBackgroundRect.left), Gdiplus::REAL(TagBackgroundRect.top + (TagHeight / 2))), &Gdiplus::StringFormat(), &FontColor);
				}

				// Drawing the leader line
				RECT TagBackRectData = TagBackgroundRect;
				POINT toDraw1, toDraw2;
				if (LiangBarsky(TagBackRectData, RtPoint, TagBackgroundRect.CenterPoint(), toDraw1, toDraw2))
					graphics.DrawLine(&Pen(Color::White), PointF(Gdiplus::REAL(RtPoint.x), Gdiplus::REAL(RtPoint.y)), PointF(Gdiplus::REAL(toDraw1.x), Gdiplus::REAL(toDraw1.y)));

				// If we use a RIMCAS label only, we display it, and adapt the rectangle
				CRect oldCrectSave = TagBackgroundRect;

				if (rimcasLabelOnly) {
					Color RimcasLabelColor = RimcasInstance->GetAircraftColor(rt.GetCallsign(), Color::AliceBlue, Color::AliceBlue,
						CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["rimcas"]["background_color_stage_one"]),
						CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["rimcas"]["background_color_stage_two"]));

					if (RimcasLabelColor.ToCOLORREF() != Color(Color::AliceBlue).ToCOLORREF()) {
						int rimcas_height = 0;

						wstring wrimcas_height = wstring(L"ALERT");

						RectF RectRimcas_height;

						graphics.MeasureString(wrimcas_height.c_str(), wcslen(wrimcas_height.c_str()), customFonts[currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &RectRimcas_height);
						rimcas_height = int(RectRimcas_height.GetBottom());

						// Drawing the rectangle

						CRect RimcasLabelRect(TagBackgroundRect.left, TagBackgroundRect.top - rimcas_height, TagBackgroundRect.right, TagBackgroundRect.top);
						graphics.FillRectangle(&SolidBrush(RimcasLabelColor), CopyRect(RimcasLabelRect));
						TagBackgroundRect.top -= rimcas_height;

						// Drawing the text

						SolidBrush FontColor(Color::White);
						wstring rimcasw = wstring(L"ALERT");
						StringFormat stformat = new StringFormat();
						stformat.SetAlignment(StringAlignment::StringAlignmentCenter);
						graphics.DrawString(rimcasw.c_str(), wcslen(rimcasw.c_str()), customFonts[currentFontSize], PointF(Gdiplus::REAL((TagBackgroundRect.left + TagBackgroundRect.right) / 2), Gdiplus::REAL(TagBackgroundRect.top)), &stformat, &FontColor);

					}
				}

				// Adding the tag screen object

				//AddScreenObject(DRAWING_TAG, rt.GetCallsign(), TagBackgroundRect, true, GetBottomLine(rt.GetCallsign()).c_str());

				TagBackgroundRect = oldCrectSave;

				// Now adding the clickable zones

				// We need to get the size of a blank space
				int blank_space_width = 0;
				CRect blankSpace(0, 0, 0, 0);
				dc.DrawText(" ", &blankSpace, DT_CALCRECT);
				blank_space_width = blankSpace.right;

				vector<string> line1_items = split(line1_size, ' ');
				int offset = 0;
				for (auto &item : line1_items)
				{
					if (TagClickableMap.find(item) != TagClickableMap.end()) {
						// We need to get the area that text covers

						if (TagClickableMap[item] == TAG_CITEM_NO)
							continue;

						int ItemWidth, ItemHeight;
						wstring item_sizew = wstring(item.begin(), item.end());

						RectF itemBox;

						graphics.MeasureString(item_sizew.c_str(), wcslen(item_sizew.c_str()), customFonts[currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &itemBox);
						ItemWidth = int(itemBox.GetRight());
						ItemHeight = int(itemBox.GetBottom() - 2);

						// We then calculate the rectangle
						CRect ItemRect(TagBackgroundRect.left + offset,
							TagBackgroundRect.top,
							(TagBackgroundRect.left + offset) + ItemWidth,
							TagBackgroundRect.top + ItemHeight);

						// If there is a squawk error and the item is a squawk error, we re-draw it with the color

						if (has_squawk_error && strcmp(item.c_str(), sqerror.c_str()) == 0) {
							SolidBrush FontColor(CurrentConfig->getConfigColor(LabelsSettings["squawk_error_color"]));
							wstring sqw = wstring(sqerror.begin(), sqerror.end());
							graphics.DrawString(sqw.c_str(), wcslen(sqw.c_str()), customFonts[currentFontSize], PointF(Gdiplus::REAL(ItemRect.left), Gdiplus::REAL(ItemRect.top)), &Gdiplus::StringFormat(), &FontColor);
						}

						// We then add the screen object
						AddScreenObject(TagClickableMap[item], rt.GetCallsign(), ItemRect, true, GetBottomLine(rt.GetCallsign()).c_str());

						// Finally, we update the offset

						offset += ItemWidth + blank_space_width;
					}
				}

				// If there is a line 2, then we do it all over again :p
				if (LabelsSettings["airborne"]["two_lines_tag"].GetBool()) {
					vector<string> line2_items = split(line2_size, ' ');
					offset = 0;
					for (auto &item : line2_items)
					{
						if (TagClickableMap.find(item) != TagClickableMap.end()) {
							// We need to get the area that text covers

							if (TagClickableMap[item] == TAG_CITEM_NO)
								continue;

							int ItemWidth, ItemHeight;
							wstring item_sizew = wstring(item.begin(), item.end());

							RectF itemBox;

							graphics.MeasureString(item_sizew.c_str(), wcslen(item_sizew.c_str()), customFonts[currentFontSize], PointF(0, 0), &Gdiplus::StringFormat(), &itemBox);
							ItemWidth = int(itemBox.GetRight());
							ItemHeight = int(itemBox.GetBottom() - 2);

							// We then calculate the rectangle
							CRect ItemRect(TagBackgroundRect.left + offset,
								TagBackgroundRect.top + (TagHeight / 2),
								(TagBackgroundRect.left + offset) + ItemWidth,
								(TagBackgroundRect.top + (TagHeight / 2)) + ItemHeight);

							// If there is a squawk error and the item is a squawk error, we re-draw it with the color

							if (has_squawk_error && strcmp(item.c_str(), sqerror.c_str()) == 0) {
								SolidBrush FontColor(CurrentConfig->getConfigColor(LabelsSettings["squawk_error_color"]));
								wstring sqw = wstring(sqerror.begin(), sqerror.end());
								graphics.DrawString(sqw.c_str(), wcslen(sqw.c_str()), customFonts[currentFontSize], PointF(Gdiplus::REAL(ItemRect.left), Gdiplus::REAL(ItemRect.top)), &Gdiplus::StringFormat(), &FontColor);
							}

							// We then add the screen object
							AddScreenObject(TagClickableMap[item], rt.GetCallsign(), ItemRect, true, GetBottomLine(rt.GetCallsign()).c_str());

							// Finally, we update the offset

							offset += ItemWidth + blank_space_width;
						}
					}
				}
			}
		}

		// Resize square
		qBackgroundColor = RGB(60, 60, 60);
		POINT BottomRight = { appWindowAreas[appWindowId].right, appWindowAreas[appWindowId].bottom };
		POINT TopLeft = { BottomRight.x - 10, BottomRight.y - 10 };
		CRect ResizeArea = { TopLeft, BottomRight };
		ResizeArea.NormalizeRect();
		dc.FillSolidRect(ResizeArea, qBackgroundColor);
		AddScreenObject(appWindowId + APPWINDOW_BASE, "resize", ResizeArea, true, "");

		CPen DarkPenResize(PS_SOLID, 1, RGB(0, 0, 0));
		dc.SelectObject(&DarkPenResize);
		dc.MoveTo(ResizeArea.TopLeft());
		dc.LineTo({ ResizeArea.left, ResizeArea.bottom });
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

		AddScreenObject(appWindowId + APPWINDOW_BASE, "topbar", TopBar, true, "");

		string Toptext = "Approach Inset " + std::to_string(appWindowId) + " (zoom: ";
		Toptext += std::to_string(appWindowScales[appWindowId]);
		Toptext += ", filter: ";
		Toptext += std::to_string(appWindowFilters[appWindowId]);
		Toptext += "ft)";
		dc.TextOutA(TopLeftText.x, TopLeftText.y, Toptext.c_str());

		int TextOffset = dc.GetTextExtent("Approach Inset 1 (zoom: ").cx;
		string temp = std::to_string(appWindowScales[appWindowId]);
		AddScreenObject(appWindowId + APPWINDOW_BASE, "range", Utils::GetAreaFromText(&dc, temp.c_str(), { TopLeftText.x + TextOffset, TopLeftText.y }), false, "");

		TextOffset += dc.GetTextExtent(temp.c_str()).cx;
		TextOffset += dc.GetTextExtent(", filter: ").cx;
		temp = std::to_string(appWindowFilters[appWindowId]);
		temp += "ft";
		AddScreenObject(appWindowId + APPWINDOW_BASE, "filter", Utils::GetAreaFromText(&dc, temp.c_str(), { TopLeftText.x + TextOffset, TopLeftText.y }), false, "");

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
		AddScreenObject(appWindowId + APPWINDOW_BASE, "close", CloseRect, false, "");
	}

	dc.Detach();

}
