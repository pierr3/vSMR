#pragma once
#include "EuroScopePlugIn.h"
#include "HttpHelper.hpp"
#include "CPDLCSettingsDialog.hpp"
#include "DataLinkDialog.hpp"
#include <string>
#include <algorithm>
#include "Mmsystem.h"
#include "SMRRadar.hpp"

#define MY_PLUGIN_NAME      "vSMR"
#define MY_PLUGIN_VERSION   "RC1-dev-e32"
#define MY_PLUGIN_DEVELOPER "Pierre Ferran"
#define MY_PLUGIN_COPYRIGHT "GPL v3"
#define MY_PLUGIN_VIEW_AVISO  "SMR radar display"

using namespace std;
using namespace EuroScopePlugIn;

class CSMRPlugin :
	public EuroScopePlugIn::CPlugIn
{
public:
	CSMRPlugin();
	virtual ~CSMRPlugin();

	const int TAG_ITEM_DATALINK_STS = 444;
	const int TAG_FUNC_DATALINK_MENU = 544;

	const int TAG_FUNC_DATALINK_CONFIRM = 545;
	const int TAG_FUNC_DATALINK_STBY = 546;
	const int TAG_FUNC_DATALINK_VOICE = 547;
	const int TAG_FUNC_DATALINK_RESET = 548;

	//---OnCompileCommand------------------------------------------

	virtual bool OnCompileCommand(const char * sCommandLine);

	//---OnFunctionCall------------------------------------------

	virtual void OnFunctionCall(int FunctionId, const char * sItemString, POINT Pt, RECT Area);

	//---OnGetTagItem------------------------------------------

	virtual void OnGetTagItem(CFlightPlan FlightPlan, CRadarTarget RadarTarget, int ItemCode, int TagData, char sItemString[16], int * pColorCode, COLORREF * pRGB, double * pFontSize);

	//---OnControllerDisconnect------------------------------------------

	virtual void OnControllerDisconnect(CController Controller);

	//---OnTimer------------------------------------------

	virtual void OnTimer(int Counter);

	//---OnRadarScreenCreated------------------------------------------

	virtual CRadarScreen * OnRadarScreenCreated(const char * sDisplayName, bool NeedRadarContent, bool GeoReferenced, bool CanBeSaved, bool CanBeCreated);
};

