#include "stdafx.h"
#include "Resource.h"
#include "SMRRadar.hpp"

RECT TimePopupArea = { 300, 300, 430, 363 };

ULONG_PTR m_gdiplusToken;
CPoint mouseLocation(0, 0);
string TagBeingDragged;
int LeaderLineDefaultlenght = 50;

//
// Cursor Things
//

bool initCursor = true;
WNDPROC gSourceProc;
HWND pluginWindow;
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

map<int, CInsetWindow *> appWindows;

inline float closest(std::vector<float> const& vec, float value) {
	auto const it = std::lower_bound(vec.begin(), vec.end(), value);
	if (it == vec.end()) { return -1; }

	return *it;
};
inline bool IsTagBeingDragged(string c)
{
	return TagBeingDragged == c;
}
bool mouseWithin(CRect rect) {
	if (mouseLocation.x >= rect.left + 1 && mouseLocation.x <= rect.right - 1 && mouseLocation.y >= rect.top + 1 && mouseLocation.y <= rect.bottom - 1)
		return true;
	return false;
}

// ReSharper disable CppMsExtAddressOfClassRValue

CSMRRadar::CSMRRadar()
{
	// Initializing randomizer
	srand(static_cast<unsigned>(time(nullptr)));

	// Initialize GDI+
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);

	// Getting the DLL file folder
	GetModuleFileNameA(HINSTANCE(&__ImageBase), DllPathFile, sizeof(DllPathFile));
	DllPath = DllPathFile;
	DllPath.resize(DllPath.size() - strlen("vSMR.dll"));

	// Loading up the callsigns for the bottom lign
	Callsigns = new CCallsignLookup(DllPath + "\\ICAO_Airlines.txt");

	// Creating the RIMCAS instance
	if (RimcasInstance == nullptr)
		RimcasInstance = new CRimcas();

	// Loading up the config file
	if (CurrentConfig == nullptr)
		CurrentConfig = new CConfig(DllPath + "\\vSMR_Profiles.json");

	SMRSharedData::standardCursor = true;
	ActiveAirport = "LFPG";

	// Setting up the data for the 2 approach windows
	appWindowDisplays[1] = false;
	appWindowDisplays[2] = false;
	appWindows[1] = new CInsetWindow(APPWINDOW_ONE);
	appWindows[2] = new CInsetWindow(APPWINDOW_TWO);

	this->CSMRRadar::LoadProfile("Default");

	this->CSMRRadar::LoadCustomFont();

	this->CSMRRadar::RefreshAirportActivity();
}

CSMRRadar::~CSMRRadar()
{
	// Shutting down GDI+
	GdiplusShutdown(m_gdiplusToken);
}

void CSMRRadar::LoadCustomFont() {
	// Loading the custom font if there is one in use
	customFonts.clear();
		
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
	customFonts[5] = new Gdiplus::Font(buffer.c_str(), Gdiplus::REAL(FSizes["five"].GetInt()), fontStyle, Gdiplus::UnitPixel);
}

void CSMRRadar::LoadProfile(string profileName) {
	// Loading the new profile
	CurrentConfig->setActiveProfile(profileName);

	// Loading all the new data
	RimcasInstance->toggleShortTimer(CurrentConfig->getActiveProfile()["rimcas"]["short_timer"].GetBool());
	LeaderLineDefaultlenght = CurrentConfig->getActiveProfile()["labels"]["leader_line_length"].GetInt();

	// Reloading the fonts
	this->LoadCustomFont();
}

// TODO: Save the trails data, afterglow, predicted line

void CSMRRadar::OnAsrContentLoaded(bool Loaded)
{
	const char * p_value;

	// ReSharper disable CppZeroConstantCanBeReplacedWithNullptr
	if ((p_value = GetDataFromAsr("Airport")) != NULL)
		setActiveAirport(p_value);

	if ((p_value = GetDataFromAsr("ActiveProfile")) != NULL)
		this->LoadProfile(string(p_value));

	if ((p_value = GetDataFromAsr("FontSize")) != NULL)
		currentFontSize = atoi(p_value);

	string temp;

	for (int i = 1; i < 3; i++)
	{
		string prefix = "SRW" + std::to_string(i);

		if ((p_value = GetDataFromAsr(string(prefix + "TopLeftX").c_str())) != NULL)
			appWindows[i]->m_Area.left = atoi(p_value);

		if ((p_value = GetDataFromAsr(string(prefix + "TopLeftY").c_str())) != NULL)
			appWindows[i]->m_Area.top = atoi(p_value);

		if ((p_value = GetDataFromAsr(string(prefix + "BottomRightX").c_str())) != NULL)
			appWindows[i]->m_Area.right = atoi(p_value);

		if ((p_value = GetDataFromAsr(string(prefix + "BottomRightY").c_str())) != NULL)
			appWindows[i]->m_Area.bottom = atoi(p_value);

		if ((p_value = GetDataFromAsr(string(prefix + "Filter").c_str())) != NULL)
			appWindows[i]->m_Filter = atoi(p_value);

		if ((p_value = GetDataFromAsr(string(prefix + "Scale").c_str())) != NULL)
			appWindows[i]->m_Scale = atoi(p_value);

		if ((p_value = GetDataFromAsr(string(prefix + "Rotation").c_str())) != NULL)
			appWindows[i]->m_Rotation = atoi(p_value);

		if ((p_value = GetDataFromAsr(string(prefix + "Display").c_str())) != NULL)
			appWindowDisplays[i] = bool(atoi(p_value));
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

	// ReSharper restore CppZeroConstantCanBeReplacedWithNullptr
}

void CSMRRadar::OnAsrContentToBeSaved(void)
{
	SaveDataToAsr("Airport", "Active airport for RIMCAS", getActiveAirport().c_str());

	SaveDataToAsr("ActiveProfile", "vSMR active profile", CurrentConfig->getActiveProfile()["name"].GetString());

	SaveDataToAsr("FontSize", "vSMR font size", std::to_string(currentFontSize).c_str());

	
	string temp;
	
	for (int i = 1; i < 3; i++)
	{
		string prefix = "SRW" + std::to_string(i);

		temp = std::to_string(appWindows[i]->m_Area.left);
		SaveDataToAsr(string(prefix + "TopLeftX").c_str(), "SRW position", temp.c_str());

		temp = std::to_string(appWindows[i]->m_Area.top);
		SaveDataToAsr(string(prefix + "TopLeftY").c_str(), "SRW position", temp.c_str());

		temp = std::to_string(appWindows[i]->m_Area.right);
		SaveDataToAsr(string(prefix + "BottomRightX").c_str(), "SRW position", temp.c_str());

		temp = std::to_string(appWindows[i]->m_Area.bottom);
		SaveDataToAsr(string(prefix + "BottomRightY").c_str(), "SRW position", temp.c_str());

		temp = std::to_string(appWindows[i]->m_Filter);
		SaveDataToAsr(string(prefix + "Filter").c_str(), "SRW filter", temp.c_str());

		temp = std::to_string(appWindows[i]->m_Scale);
		SaveDataToAsr(string(prefix + "Scale").c_str(), "SRW range", temp.c_str());

		temp = std::to_string(appWindows[i]->m_Rotation);
		SaveDataToAsr(string(prefix + "Rotation").c_str(), "SRW rotation", temp.c_str());

		string to_save = "0";
		if (appWindowDisplays[i])
			to_save = "1";
		SaveDataToAsr(string(prefix + "Display").c_str(), "Display Secondary Radar Window", to_save.c_str());
	}

}

void CSMRRadar::OnMoveScreenObject(int ObjectType, const char * sObjectId, POINT Pt, RECT Area, bool Released) {

	if (ObjectType == APPWINDOW_ONE || ObjectType == APPWINDOW_TWO) {
		int appWindowId = ObjectType - APPWINDOW_BASE;

		bool toggleCursor = appWindows[appWindowId]->OnMoveScreenObject(sObjectId, Pt, Area, Released);
		
		if (!toggleCursor)
		{
			if (SMRSharedData::standardCursor)
			{
				if (strcmp(sObjectId, "topbar") == 0)
					SMRSharedData::smrCursor = CopyCursor((HCURSOR)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_SMRMOVEWINDOW), IMAGE_CURSOR, 0, 0, LR_SHARED));
				else if (strcmp(sObjectId, "resize") == 0)
					SMRSharedData::smrCursor = CopyCursor((HCURSOR)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_SMRRESIZE), IMAGE_CURSOR, 0, 0, LR_SHARED));

				AfxGetMainWnd()->SendMessage(WM_SETCURSOR);
				SMRSharedData::standardCursor = false;
			}
		} else
		{
			if (!SMRSharedData::standardCursor)
			{
				SMRSharedData::smrCursor = CopyCursor((HCURSOR)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_SMRCURSOR), IMAGE_CURSOR, 0, 0, LR_SHARED));
				AfxGetMainWnd()->SendMessage(WM_SETCURSOR);
				SMRSharedData::standardCursor = true;
			}
		}
	}

	if (ObjectType == DRAWING_TAG || ObjectType == TAG_CITEM_MANUALCORRELATE ||ObjectType == TAG_CITEM_CALLSIGN || ObjectType == TAG_CITEM_FPBOX || ObjectType == TAG_CITEM_RWY || ObjectType == TAG_CITEM_GATE) {
		CRadarTarget rt = GetPlugIn()->RadarTargetSelect(sObjectId);

		if (!Released)
		{
			if(SMRSharedData::standardCursor)
			{
				SMRSharedData::smrCursor = CopyCursor((HCURSOR)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_SMRMOVETAG), IMAGE_CURSOR, 0, 0, LR_SHARED));
				AfxGetMainWnd()->SendMessage(WM_SETCURSOR);
				SMRSharedData::standardCursor = false;
			}
		} else
		{
			if (!SMRSharedData::standardCursor)
			{
				SMRSharedData::smrCursor = CopyCursor((HCURSOR)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_SMRCURSOR), IMAGE_CURSOR, 0, 0, LR_SHARED));
				AfxGetMainWnd()->SendMessage(WM_SETCURSOR);
				SMRSharedData::standardCursor = true;
			}
		}
		
		if (rt.IsValid()) {
			CRect Temp = Area;
			POINT TagCenterPix = Temp.CenterPoint();
			POINT AcPosPix = ConvertCoordFromPositionToPixel(GetPlugIn()->RadarTargetSelect(sObjectId).GetPosition().GetPosition());
			POINT CustomTag = { TagCenterPix.x - AcPosPix.x, TagCenterPix.y - AcPosPix.y };
			
			if (CurrentConfig->getActiveProfile()["labels"]["auto_deconfliction"].GetBool())
			{
				double angle = RadToDeg(atan2(CustomTag.y, CustomTag.x));
				angle = fmod(angle + 360, 360);
				vector<float> angles;
				for (float k = 0.0f; k <= 360.0f; k += 22.5f)
					angles.push_back(k);

				TagAngles[sObjectId] = closest(angles, angle);
				TagLeaderLineLength[sObjectId] = max(LeaderLineDefaultlenght, min(DistancePts(AcPosPix, TagCenterPix), LeaderLineDefaultlenght * 2));

			} else
			{
				TagsOffsets[sObjectId] = CustomTag;
			}

			
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
	}

	mouseLocation = Pt;
	RequestRefresh();

}

void CSMRRadar::OnOverScreenObject(int ObjectType, const char * sObjectId, POINT Pt, RECT Area)
{
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
			GetPlugIn()->OpenPopupList(Area, "SRW Zoom", 1);
			GetPlugIn()->AddPopupListElement("55", "", RIMCAS_UPDATERANGE + appWindowId, false, int(appWindows[appWindowId]->m_Scale == 55));
			GetPlugIn()->AddPopupListElement("50", "", RIMCAS_UPDATERANGE + appWindowId, false, int(appWindows[appWindowId]->m_Scale == 50));
			GetPlugIn()->AddPopupListElement("45", "", RIMCAS_UPDATERANGE + appWindowId, false, int(appWindows[appWindowId]->m_Scale == 45));
			GetPlugIn()->AddPopupListElement("40", "", RIMCAS_UPDATERANGE + appWindowId, false, int(appWindows[appWindowId]->m_Scale == 40));
			GetPlugIn()->AddPopupListElement("35", "", RIMCAS_UPDATERANGE + appWindowId, false, int(appWindows[appWindowId]->m_Scale == 35));
			GetPlugIn()->AddPopupListElement("30", "", RIMCAS_UPDATERANGE + appWindowId, false, int(appWindows[appWindowId]->m_Scale == 30));
			GetPlugIn()->AddPopupListElement("25", "", RIMCAS_UPDATERANGE + appWindowId, false, int(appWindows[appWindowId]->m_Scale == 25));
			GetPlugIn()->AddPopupListElement("20", "", RIMCAS_UPDATERANGE + appWindowId, false, int(appWindows[appWindowId]->m_Scale == 20));
			GetPlugIn()->AddPopupListElement("15", "", RIMCAS_UPDATERANGE + appWindowId, false, int(appWindows[appWindowId]->m_Scale == 15));
			GetPlugIn()->AddPopupListElement("10", "", RIMCAS_UPDATERANGE + appWindowId, false, int(appWindows[appWindowId]->m_Scale == 10));
			GetPlugIn()->AddPopupListElement("5", "", RIMCAS_UPDATERANGE + appWindowId, false, int(appWindows[appWindowId]->m_Scale == 5));
			GetPlugIn()->AddPopupListElement("1", "", RIMCAS_UPDATERANGE + appWindowId, false, int(appWindows[appWindowId]->m_Scale == 1));
			GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		}
		if (strcmp(sObjectId, "filter") == 0) {
			GetPlugIn()->OpenPopupList(Area, "SRW Filter (ft)", 1);
			GetPlugIn()->AddPopupListElement("UNL", "", RIMCAS_UPDATEFILTER + appWindowId, false, int(appWindows[appWindowId]->m_Filter == 66000));
			GetPlugIn()->AddPopupListElement("9500", "", RIMCAS_UPDATEFILTER + appWindowId, false, int(appWindows[appWindowId]->m_Filter == 9500));
			GetPlugIn()->AddPopupListElement("8500", "", RIMCAS_UPDATEFILTER + appWindowId, false, int(appWindows[appWindowId]->m_Filter == 8500));
			GetPlugIn()->AddPopupListElement("7500", "", RIMCAS_UPDATEFILTER + appWindowId, false, int(appWindows[appWindowId]->m_Filter == 7500));
			GetPlugIn()->AddPopupListElement("6500", "", RIMCAS_UPDATEFILTER + appWindowId, false, int(appWindows[appWindowId]->m_Filter == 6500));
			GetPlugIn()->AddPopupListElement("5500", "", RIMCAS_UPDATEFILTER + appWindowId, false, int(appWindows[appWindowId]->m_Filter == 5500));
			GetPlugIn()->AddPopupListElement("4500", "", RIMCAS_UPDATEFILTER + appWindowId, false, int(appWindows[appWindowId]->m_Filter == 4500));
			GetPlugIn()->AddPopupListElement("3500", "", RIMCAS_UPDATEFILTER + appWindowId, false, int(appWindows[appWindowId]->m_Filter == 3500));
			GetPlugIn()->AddPopupListElement("2500", "", RIMCAS_UPDATEFILTER + appWindowId, false, int(appWindows[appWindowId]->m_Filter == 2500));
			GetPlugIn()->AddPopupListElement("1500", "", RIMCAS_UPDATEFILTER + appWindowId, false, int(appWindows[appWindowId]->m_Filter == 1500));
			GetPlugIn()->AddPopupListElement("500", "", RIMCAS_UPDATEFILTER + appWindowId, false, int(appWindows[appWindowId]->m_Filter == 500));
			string tmp = std::to_string(GetPlugIn()->GetTransitionAltitude());
			GetPlugIn()->AddPopupListElement(tmp.c_str(), "", RIMCAS_UPDATEFILTER + appWindowId, false, 2, false, true);
			GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		}
		if (strcmp(sObjectId, "rotate") == 0) {
			GetPlugIn()->OpenPopupList(Area, "SRW Rotate (deg)", 1);
			for (int k = 0; k <= 360; k = k+2)
			{
				string tmp = std::to_string(k);
				GetPlugIn()->AddPopupListElement(tmp.c_str(), "", RIMCAS_UPDATEROTATE + appWindowId, false, int(appWindows[appWindowId]->m_Rotation == k));
			}
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
			GetPlugIn()->AddPopupListElement("SRW 1", "", APPWINDOW_ONE, false, int(appWindowDisplays[1]));
			GetPlugIn()->AddPopupListElement("SRW 2", "", APPWINDOW_TWO, false, int(appWindowDisplays[2]));
			GetPlugIn()->AddPopupListElement("Profiles", "", RIMCAS_OPEN_LIST);
			GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		}

		if (strcmp(sObjectId, "TargetMenu") == 0) {
			Area.top = Area.top + 30;
			Area.bottom = Area.bottom + 30;

			GetPlugIn()->OpenPopupList(Area, "Target", 1);
			GetPlugIn()->AddPopupListElement("Label Font Size", "", RIMCAS_OPEN_LIST);
			GetPlugIn()->AddPopupListElement("Afterglow", "", RIMCAS_UPDATE_AFTERGLOW, false, int(Afterglow));
			GetPlugIn()->AddPopupListElement("GRND Trail Dots", "", RIMCAS_OPEN_LIST);
			GetPlugIn()->AddPopupListElement("APPR Trail Dots", "", RIMCAS_OPEN_LIST);
			GetPlugIn()->AddPopupListElement("Predicted Track Line", "", RIMCAS_OPEN_LIST);
			GetPlugIn()->AddPopupListElement("Acquire", "", RIMCAS_UPDATE_ACQUIRE);
			GetPlugIn()->AddPopupListElement("Release", "", RIMCAS_UPDATE_RELEASE);
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

		// Release & correlate actions

		if (ReleaseInProgress || AcquireInProgress)
		{
			if (ReleaseInProgress)
			{
				ReleaseInProgress = NeedCorrelateCursor = false;

				ReleasedTracks.push_back(rt.GetSystemID());

				if (std::find(ManuallyCorrelated.begin(), ManuallyCorrelated.end(), rt.GetSystemID()) != ManuallyCorrelated.end())
					ManuallyCorrelated.erase(std::find(ManuallyCorrelated.begin(), ManuallyCorrelated.end(), rt.GetSystemID()));
			}

			if (AcquireInProgress)
			{
				AcquireInProgress = NeedCorrelateCursor = false;

				ManuallyCorrelated.push_back(rt.GetSystemID());

				if (std::find(ReleasedTracks.begin(), ReleasedTracks.end(), rt.GetSystemID()) != ReleasedTracks.end())
					ReleasedTracks.erase(std::find(ReleasedTracks.begin(), ReleasedTracks.end(), rt.GetSystemID()));
			}


			if (NeedCorrelateCursor)
			{
				if (SMRSharedData::standardCursor)
				{
					SMRSharedData::smrCursor = CopyCursor((HCURSOR)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_SMRCORRELATE), IMAGE_CURSOR, 0, 0, LR_SHARED));
					AfxGetMainWnd()->SendMessage(WM_SETCURSOR);
					SMRSharedData::standardCursor = false;
				}
			}
			else
			{
				if (!SMRSharedData::standardCursor)
				{
					SMRSharedData::smrCursor = CopyCursor((HCURSOR)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_SMRCURSOR), IMAGE_CURSOR, 0, 0, LR_SHARED));
					AfxGetMainWnd()->SendMessage(WM_SETCURSOR);
					SMRSharedData::standardCursor = true;
				}
			}
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
	if (FunctionId == APPWINDOW_ONE || FunctionId == APPWINDOW_TWO) {
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
		if (strcmp(sItemString, "Size 5") == 0)
			currentFontSize = 5;

		ShowLists["Label Font Size"] = true;
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
		this->CSMRRadar::LoadProfile(sItemString);
		LoadCustomFont();
		SaveDataToAsr("ActiveProfile", "vSMR active profile", sItemString);

		ShowLists["Profiles"] = true;
	}

	if (FunctionId == RIMCAS_UPDATEFILTER1 || FunctionId == RIMCAS_UPDATEFILTER2) {
		int id = FunctionId - RIMCAS_UPDATEFILTER;
		if (startsWith("UNL", sItemString))
			sItemString = "66000";
		appWindows[id]->m_Filter = atoi(sItemString);
	}

	if (FunctionId == RIMCAS_UPDATERANGE1 || FunctionId == RIMCAS_UPDATERANGE2) {
		int id = FunctionId - RIMCAS_UPDATERANGE;
		appWindows[id]->m_Scale = atoi(sItemString);
	}

	if (FunctionId == RIMCAS_UPDATEROTATE1 || FunctionId == RIMCAS_UPDATEROTATE2) {
		int id = FunctionId - RIMCAS_UPDATEROTATE;
		appWindows[id]->m_Rotation = atoi(sItemString);
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

	if (FunctionId == RIMCAS_UPDATE_AFTERGLOW)
	{
		Afterglow = !Afterglow;
	}

	if (FunctionId == RIMCAS_UPDATE_GND_TRAIL)
	{
		Trail_Gnd = atoi(sItemString);

		ShowLists["GRND Trail Dots"] = true;
	}

	if (FunctionId == RIMCAS_UPDATE_APP_TRAIL)
	{
		Trail_App = atoi(sItemString);

		ShowLists["APPR Trail Dots"] = true;
	}

	if (FunctionId == RIMCAS_UPDATE_PTL)
	{
		PredictedLenght = atoi(sItemString);

		ShowLists["Predicted Track Line"] = true;
	}

	if (FunctionId == RIMCAS_UPDATE_RELEASE)
	{
		ReleaseInProgress = !ReleaseInProgress;
		if (ReleaseInProgress)
			AcquireInProgress = false;
		NeedCorrelateCursor = ReleaseInProgress;

		if (NeedCorrelateCursor)
		{
			if (SMRSharedData::standardCursor)
			{
				SMRSharedData::smrCursor = CopyCursor((HCURSOR)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_SMRCORRELATE), IMAGE_CURSOR, 0, 0, LR_SHARED));
				AfxGetMainWnd()->SendMessage(WM_SETCURSOR);
				SMRSharedData::standardCursor = false;
			}
		}
		else
		{
			if (!SMRSharedData::standardCursor)
			{
				SMRSharedData::smrCursor = CopyCursor((HCURSOR)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_SMRCURSOR), IMAGE_CURSOR, 0, 0, LR_SHARED));
				AfxGetMainWnd()->SendMessage(WM_SETCURSOR);
				SMRSharedData::standardCursor = true;
			}
		}
	}

	if (FunctionId == RIMCAS_UPDATE_ACQUIRE)
	{
		AcquireInProgress = !AcquireInProgress;
		if (AcquireInProgress)
			ReleaseInProgress = false;
		NeedCorrelateCursor = AcquireInProgress;

		if (NeedCorrelateCursor)
		{
			if (SMRSharedData::standardCursor)
			{
				SMRSharedData::smrCursor = CopyCursor((HCURSOR)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_SMRCORRELATE), IMAGE_CURSOR, 0, 0, LR_SHARED));
				AfxGetMainWnd()->SendMessage(WM_SETCURSOR);
				SMRSharedData::standardCursor = false;
			}
		}
		else
		{
			if (!SMRSharedData::standardCursor)
			{
				SMRSharedData::smrCursor = CopyCursor((HCURSOR)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_SMRCURSOR), IMAGE_CURSOR, 0, 0, LR_SHARED));
				AfxGetMainWnd()->SendMessage(WM_SETCURSOR);
				SMRSharedData::standardCursor = true;
			}
		}
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
		LoadProfile(CurrentConfig->getActiveProfileName());
		return true;
	}

	return false;
}

map<string, string> CSMRRadar::GenerateTagData(CRadarTarget rt, CFlightPlan fp, int TransitionAltitude, bool useSpeedForGates)
{
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

	string actype = "NoFPL";
	if (fp.IsValid())
		actype = fp.GetFlightPlanData().GetAircraftFPType();
	if (actype.size() > 4 && actype != "NoFPL")
		actype = actype.substr(0, 4);

	// ----- Aircraft type that changes to squawk error -------
	string sctype = actype;
	if (has_squawk_error)
		sctype = sqerror;

	// ----- Groundspeed -------
	string speed = std::to_string(rt.GetPosition().GetReportedGS());

	// ----- Departure runway -------
	string deprwy = fp.GetFlightPlanData().GetDepartureRwy();
	if (deprwy.length() == 0)
		deprwy = "RWY";

	// ----- Departure runway that changes for overspeed -------
	string seprwy = deprwy;
	if (rt.GetPosition().GetReportedGS() > 25)
		seprwy = std::to_string(rt.GetPosition().GetReportedGS());

	// ----- Gate -------
	string gate;
	if (useSpeedForGates)
		gate = std::to_string(fp.GetControllerAssignedData().GetAssignedSpeed());
	else
		gate = fp.GetControllerAssignedData().GetScratchPadString();

	gate = gate.substr(0, 4);

	if (gate.size() == 0 || gate == "0")
		gate = "NoGATE";

	// ----- Gate that changes to speed -------
	string sate = gate;
	if (rt.GetPosition().GetReportedGS() > 25)
		sate = speed;

	// ----- Flightlevel -------
	int fl = rt.GetPosition().GetFlightLevel();
	int padding = 5;
	string pfls = "";
	if (fl <= TransitionAltitude) {
		fl = rt.GetPosition().GetPressureAltitude();
		pfls = "A";
		padding = 4;
	}
	string flightlevel = (pfls + padWithZeros(padding, fl)).substr(0, 3);

	// ----- Tendency -------
	string tendency = "-";
	int delta_fl = rt.GetPosition().GetFlightLevel() - rt.GetPreviousPosition(rt.GetPosition()).GetPressureAltitude();
	if (abs(delta_fl) >= 50) {
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
	string dep = "SID";
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
	TagReplacingMap["asid"] = dep;
	TagReplacingMap["ssid"] = sdep;

	// System ID for uncorrelated
	TagReplacingMap["systemid"] = "T:";
	string tpss = rt.GetSystemID();
	TagReplacingMap["systemid"].append(tpss.substr(1, 6));
	
	return TagReplacingMap;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SETCURSOR:
		SetCursor(SMRSharedData::smrCursor);
		return true;
	default:
		return CallWindowProc(gSourceProc, hwnd, uMsg, wParam, lParam);
	}
}

void CSMRRadar::OnRefresh(HDC hDC, int Phase)
{
	// Changing the mouse cursor
	if (initCursor)
	{
		SMRSharedData::smrCursor = CopyCursor((HCURSOR)::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_SMRCURSOR), IMAGE_CURSOR, 0, 0, LR_SHARED));

		if (SMRSharedData::smrCursor != nullptr)
		{
			pluginWindow = GetActiveWindow();
			gSourceProc = (WNDPROC)SetWindowLong(pluginWindow, GWL_WNDPROC, (LONG)WindowProc);
		}
		initCursor = false;
	}

	if (Phase == REFRESH_PHASE_AFTER_LISTS) {
		if (!ColorSettingsDay) {
			// Creating the gdi+ graphics
			Graphics graphics(hDC);
			graphics.SetPageUnit(Gdiplus::UnitPixel);

			graphics.SetSmoothingMode(SmoothingModeAntiAlias);

			SolidBrush AlphaBrush(Color(CurrentConfig->getActiveProfile()["filters"]["night_alpha_setting"].GetInt(), 0, 0, 0));

			CRect RadarArea(GetRadarArea());
			RadarArea.top = RadarArea.top - 1;
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
							CPosition position;
							position.LoadFromStrings(Path[j][(SizeType)1].GetString(), Path[j][(SizeType)0].GetString());

							def.push_back(position);
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
									CPosition position;
									position.LoadFromStrings(Path[w][static_cast<SizeType>(1)].GetString(), Path[w][static_cast<SizeType>(0)].GetString());

									POINT cv = ConvertCoordFromPositionToPixel(position);
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

				if (i == 1 && !Patatoides[rt.GetCallsign()].History_one_points.empty() && Afterglow && CurrentConfig->getActiveProfile()["targets"]["show_primary_target"].GetBool()) {
					SolidBrush H_Brush(CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["targets"]["history_one_color"]));
					
					PointF lpPoints[100];
					for (unsigned int i1 = 0; i1 < Patatoides[rt.GetCallsign()].History_one_points.size(); i1++)
					{
						CPosition pos;
						pos.m_Latitude = Patatoides[rt.GetCallsign()].History_one_points[i1].x;
						pos.m_Longitude = Patatoides[rt.GetCallsign()].History_one_points[i1].y;

						lpPoints[i1] = { REAL(ConvertCoordFromPositionToPixel(pos).x), REAL(ConvertCoordFromPositionToPixel(pos).y) };
					}
					graphics.FillPolygon(&H_Brush, lpPoints, Patatoides[rt.GetCallsign()].History_one_points.size());
				}

				if (i != 2) {
					if (!Patatoides[rt.GetCallsign()].History_two_points.empty() && Afterglow && CurrentConfig->getActiveProfile()["targets"]["show_primary_target"].GetBool()) {
						SolidBrush H_Brush(CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["targets"]["history_two_color"]));

						PointF lpPoints[100];
						for (unsigned int i1 = 0; i1 < Patatoides[rt.GetCallsign()].History_two_points.size(); i1++)
						{
							CPosition pos;
							pos.m_Latitude = Patatoides[rt.GetCallsign()].History_two_points[i1].x;
							pos.m_Longitude = Patatoides[rt.GetCallsign()].History_two_points[i1].y;

							lpPoints[i1] = { REAL(ConvertCoordFromPositionToPixel(pos).x), REAL(ConvertCoordFromPositionToPixel(pos).y) };
						}
						graphics.FillPolygon(&H_Brush, lpPoints, Patatoides[rt.GetCallsign()].History_two_points.size());
					}
				}

				if (i == 2 && !Patatoides[rt.GetCallsign()].History_three_points.empty() && Afterglow && CurrentConfig->getActiveProfile()["targets"]["show_primary_target"].GetBool()) {
					SolidBrush H_Brush(CurrentConfig->getConfigColor(CurrentConfig->getActiveProfile()["targets"]["history_three_color"]));

					PointF lpPoints[100];
					for (unsigned int i1 = 0; i1 < Patatoides[rt.GetCallsign()].History_three_points.size(); i1++)
					{
						CPosition pos;
						pos.m_Latitude = Patatoides[rt.GetCallsign()].History_three_points[i1].x;
						pos.m_Longitude = Patatoides[rt.GetCallsign()].History_three_points[i1].y;

						lpPoints[i1] = { REAL(ConvertCoordFromPositionToPixel(pos).x), REAL(ConvertCoordFromPositionToPixel(pos).y) };
					}
					graphics.FillPolygon(&H_Brush, lpPoints, Patatoides[rt.GetCallsign()].History_three_points.size());
				}
			}

			CPen qTrailDotsPen(PS_SOLID, 1, RGB(255, 255, 255));
			CPen* pqOrigDotsPen = dc.SelectObject(&qTrailDotsPen);

			int TrailNumber = Trail_Gnd;
			if (reportedGs > 50)
				TrailNumber = Trail_App;

			CRadarTargetPositionData previousPos = rt.GetPreviousPosition(rt.GetPosition());
			for (int j = 1; j <= TrailNumber; j++) {
				POINT pCoord = ConvertCoordFromPositionToPixel(previousPos.GetPosition());

				dc.Rectangle(pCoord.x - 1, pCoord.y - 1, pCoord.x + 1, pCoord.y + 1);

				previousPos = rt.GetPreviousPosition(previousPos);
			}

			dc.SelectObject(pqOrigDotsPen);
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

		if (!AcisCorrelated && reportedGs < 1 && !ReleaseInProgress && !AcquireInProgress)
			continue;

		CPen qTrailPen(PS_SOLID, 1, RGB(255, 255, 255));
		CPen* pqOrigPen = dc.SelectObject(&qTrailPen);

		if (RtPos.GetTransponderC()) {
			dc.MoveTo({ acPosPix.x, acPosPix.y - 6 });
			dc.LineTo({ acPosPix.x - 6, acPosPix.y });
			dc.LineTo({ acPosPix.x, acPosPix.y + 6 });
			dc.LineTo({ acPosPix.x + 6, acPosPix.y });
			dc.LineTo({ acPosPix.x, acPosPix.y - 6 });
		}
		else {
			dc.MoveTo(acPosPix.x, acPosPix.y);
			dc.LineTo(acPosPix.x - 4, acPosPix.y - 4);
			dc.MoveTo(acPosPix.x, acPosPix.y);
			dc.LineTo(acPosPix.x + 4, acPosPix.y - 4);
			dc.MoveTo(acPosPix.x, acPosPix.y);
			dc.LineTo(acPosPix.x - 4, acPosPix.y + 4);
			dc.MoveTo(acPosPix.x, acPosPix.y);
			dc.LineTo(acPosPix.x + 4, acPosPix.y + 4);
		}

		// Predicted Track Line
		// It starts 20 seconds away from the ac
		if (reportedGs > 50)
		{
			double d = double(rt.GetPosition().GetReportedGS()*0.514444) * 20;
			CPosition AwayBase = BetterHarversine(rt.GetPosition().GetPosition(), rt.GetTrackHeading(), d);

			d = double(rt.GetPosition().GetReportedGS()*0.514444) * (PredictedLenght * 60) - 20;
			CPosition PredictedEnd = BetterHarversine(AwayBase, rt.GetTrackHeading(), d);

			dc.MoveTo(ConvertCoordFromPositionToPixel(AwayBase));
			dc.LineTo(ConvertCoordFromPositionToPixel(PredictedEnd));
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

		if (std::find(ReleasedTracks.begin(), ReleasedTracks.end(), rt.GetSystemID()) != ReleasedTracks.end())
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

		map<string, string> TagReplacingMap = GenerateTagData(rt, fp, GetPlugIn()->GetTransitionAltitude(), CurrentConfig->getActiveProfile()["labels"]["use_aspeed_for_gate"].GetBool());

		// ----- Generating the clickable map -----
		map<string, int> TagClickableMap;
		TagClickableMap[TagReplacingMap["callsign"]] = TAG_CITEM_CALLSIGN;
		TagClickableMap[TagReplacingMap["actype"]] = TAG_CITEM_FPBOX;
		TagClickableMap[TagReplacingMap["sctype"]] = TAG_CITEM_FPBOX;
		TagClickableMap[TagReplacingMap["sqerror"]] = TAG_CITEM_FPBOX;
		TagClickableMap[TagReplacingMap["deprwy"]] = TAG_CITEM_RWY;
		TagClickableMap[TagReplacingMap["seprwy"]] = TAG_CITEM_RWY;
		TagClickableMap[TagReplacingMap["gate"]] = TAG_CITEM_GATE;
		TagClickableMap[TagReplacingMap["sate"]] = TAG_CITEM_GATE;
		TagClickableMap[TagReplacingMap["flightlevel"]] = TAG_CITEM_NO;
		TagClickableMap[TagReplacingMap["speed"]] = TAG_CITEM_NO;
		TagClickableMap[TagReplacingMap["tendency"]] = TAG_CITEM_NO;
		TagClickableMap[TagReplacingMap["wake"]] = TAG_CITEM_FPBOX;
		TagClickableMap[TagReplacingMap["tssr"]] = TAG_CITEM_NO;
		TagClickableMap[TagReplacingMap["asid"]] = TagClickableMap[TagReplacingMap["ssid"]] = TAG_CITEM_NO;
		TagClickableMap[TagReplacingMap["systemid"]] = TAG_CITEM_NO;

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

		for (std::map<string, string>::iterator iterator = TagReplacingMap.begin(); iterator != TagReplacingMap.end(); ++iterator)
		{
			replaceAll(line1_size, iterator->first, iterator->second);

			if (LabelsSettings[Utils2::getEnumString(TagType).c_str()]["two_lines_tag"].GetBool()) {
				replaceAll(line2_size, iterator->first, iterator->second);
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
				
				SolidBrush WhiteBrush(Color::White);
				wstring rimcasw = wstring(L"ALERT");
				StringFormat stformat = new StringFormat();
				stformat.SetAlignment(StringAlignment::StringAlignmentCenter);
				graphics.DrawString(rimcasw.c_str(), wcslen(rimcasw.c_str()), customFonts[currentFontSize], PointF(Gdiplus::REAL((TagBackgroundRect.left + TagBackgroundRect.right) / 2), Gdiplus::REAL(TagBackgroundRect.top)), &stformat, &WhiteBrush);
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

				if (TagReplacingMap["sqerror"].size() > 0 && strcmp(item.c_str(), TagReplacingMap["sqerror"].c_str()) == 0) {
					SolidBrush TextColor(CurrentConfig->getConfigColor(LabelsSettings["squawk_error_color"]));
					wstring sqw = wstring(TagReplacingMap["sqerror"].begin(), TagReplacingMap["sqerror"].end());
					graphics.DrawString(sqw.c_str(), wcslen(sqw.c_str()), customFonts[currentFontSize], PointF(Gdiplus::REAL(ItemRect.left), Gdiplus::REAL(ItemRect.top)), &Gdiplus::StringFormat(), &TextColor);
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

					if (TagReplacingMap["sqerror"].size() > 0 && strcmp(item.c_str(), TagReplacingMap["sqerror"].c_str()) == 0) {
							SolidBrush TextColor(CurrentConfig->getConfigColor(LabelsSettings["squawk_error_color"]));
							wstring sqw = wstring(TagReplacingMap["sqerror"].begin(), TagReplacingMap["sqerror"].end());
							graphics.DrawString(sqw.c_str(), wcslen(sqw.c_str()), customFonts[currentFontSize], PointF(Gdiplus::REAL(ItemRect.left), Gdiplus::REAL(ItemRect.top)), &Gdiplus::StringFormat(), &TextColor);
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

	if (ShowLists["Label Font Size"]) {
		GetPlugIn()->OpenPopupList(ListAreas["Label Font Size"], "Label Font Size", 1);
		GetPlugIn()->AddPopupListElement("Size 1", "", RIMCAS_UPDATE_FONTS, false, int(bool(currentFontSize == 1)));
		GetPlugIn()->AddPopupListElement("Size 2", "", RIMCAS_UPDATE_FONTS, false, int(bool(currentFontSize == 2)));
		GetPlugIn()->AddPopupListElement("Size 3", "", RIMCAS_UPDATE_FONTS, false, int(bool(currentFontSize == 3)));
		GetPlugIn()->AddPopupListElement("Size 4", "", RIMCAS_UPDATE_FONTS, false, int(bool(currentFontSize == 4)));
		GetPlugIn()->AddPopupListElement("Size 5", "", RIMCAS_UPDATE_FONTS, false, int(bool(currentFontSize == 5)));
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		ShowLists["Label Font Size"] = false;
	}

	if (ShowLists["GRND Trail Dots"]) {
		GetPlugIn()->OpenPopupList(ListAreas["GRND Trail Dots"], "GRND Trail Dots", 1);
		GetPlugIn()->AddPopupListElement("0", "", RIMCAS_UPDATE_GND_TRAIL, false, int(bool(Trail_Gnd == 0)));
		GetPlugIn()->AddPopupListElement("2", "", RIMCAS_UPDATE_GND_TRAIL, false, int(bool(Trail_Gnd == 2)));
		GetPlugIn()->AddPopupListElement("4", "", RIMCAS_UPDATE_GND_TRAIL, false, int(bool(Trail_Gnd == 4)));
		GetPlugIn()->AddPopupListElement("8", "", RIMCAS_UPDATE_GND_TRAIL, false, int(bool(Trail_Gnd == 8)));
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		ShowLists["GRND Trail Dots"] = false;
	}

	if (ShowLists["APPR Trail Dots"]) {
		GetPlugIn()->OpenPopupList(ListAreas["APPR Trail Dots"], "APPR Trail Dots", 1);
		GetPlugIn()->AddPopupListElement("0", "", RIMCAS_UPDATE_APP_TRAIL, false, int(bool(Trail_App == 0)));
		GetPlugIn()->AddPopupListElement("4", "", RIMCAS_UPDATE_APP_TRAIL, false, int(bool(Trail_App == 4)));
		GetPlugIn()->AddPopupListElement("8", "", RIMCAS_UPDATE_APP_TRAIL, false, int(bool(Trail_App == 8)));
		GetPlugIn()->AddPopupListElement("12", "", RIMCAS_UPDATE_APP_TRAIL, false, int(bool(Trail_App == 12)));
		GetPlugIn()->AddPopupListElement("16", "", RIMCAS_UPDATE_APP_TRAIL, false, int(bool(Trail_App == 16)));
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		ShowLists["APPR Trail Dots"] = false;
	}

	if (ShowLists["Predicted Track Line"]) {
		GetPlugIn()->OpenPopupList(ListAreas["Predicted Track Line"], "Predicted Track Line", 1);
		GetPlugIn()->AddPopupListElement("0", "", RIMCAS_UPDATE_PTL, false, int(bool(PredictedLenght == 0)));
		GetPlugIn()->AddPopupListElement("1", "", RIMCAS_UPDATE_PTL, false, int(bool(PredictedLenght == 1)));
		GetPlugIn()->AddPopupListElement("2", "", RIMCAS_UPDATE_PTL, false, int(bool(PredictedLenght == 2)));
		GetPlugIn()->AddPopupListElement("3", "", RIMCAS_UPDATE_PTL, false, int(bool(PredictedLenght == 3)));
		GetPlugIn()->AddPopupListElement("4", "", RIMCAS_UPDATE_PTL, false, int(bool(PredictedLenght == 4)));
		GetPlugIn()->AddPopupListElement("5", "", RIMCAS_UPDATE_PTL, false, int(bool(PredictedLenght == 5)));
		GetPlugIn()->AddPopupListElement("Close", "", RIMCAS_CLOSE, false, 2, false, true);
		ShowLists["Predicted Track Line"] = false;
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
		POINT point;
		if (GetCursorPos(&point))
		{
			dc.LineTo(point);

			CPosition CursorPos = ConvertCoordFromPixelToPosition(point);
			double Distance = AirportCPos.DistanceTo(CursorPos);
			double Bearing = AirportCPos.DirectionTo(CursorPos);

			TGraphics th;
			th.DrawEllipse(dc, point.x - 5, point.y - 5, point.x + 5, point.y + 5, RGB(255, 255, 255));

			Distance = Distance / 0.00053996f;

			Distance = round(Distance * 10) / 10;

			Bearing = round(Bearing * 10) / 10;

			POINT TextPos = { point.x + 20, point.y };

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
			COLORREF old_color = dc.SetTextColor(RGB(255, 255, 255));
			dc.TextOutA(TextPos.x, TextPos.y, text.c_str());
			dc.SetTextColor(old_color);
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
	dc.TextOutA(ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, "Target");
	AddScreenObject(RIMCAS_MENU, "TargetMenu", { ToolBarAreaTop.left + offset, ToolBarAreaTop.top + 4, ToolBarAreaTop.left + offset + dc.GetTextExtent("Target").cx, ToolBarAreaTop.top + 4 + dc.GetTextExtent("Target").cy }, false, "Target menu");

	offset += dc.GetTextExtent("Target").cx + 10;
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
		if (!CurrentConfig->getActiveProfile()["labels"]["auto_deconfliction"].GetBool())
			break;

		if (TagsOffsets.find(areas.first) != TagsOffsets.end())
			continue;

		if (IsTagBeingDragged(areas.first))
			continue;

		if (RecentlyAutoMovedTags.find(areas.first) != RecentlyAutoMovedTags.end())
		{
			double t = (double)clock() - RecentlyAutoMovedTags[areas.first] / ((double)CLOCKS_PER_SEC);
			if (t >= 0.8) {
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

			if (IsTagBeingDragged(area2.first))
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

				if (IsTagBeingDragged(area2.first))
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
		appWindows[appWindowId]->render(hDC, this, &graphics, mouseLocation);
	}

	dc.Detach();

}

// ReSharper restore CppMsExtAddressOfClassRValue

//---EuroScopePlugInExit-----------------------------------------------

void __declspec (dllexport) EuroScopePlugInExit(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

		if (SMRSharedData::smrCursor != nullptr)
		{
			SetWindowLong(pluginWindow, GWL_WNDPROC, (LONG)gSourceProc);
		}

	stopVStripsConnection = true;
}