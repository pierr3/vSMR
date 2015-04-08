#include "stdafx.h"
#include "SMRPlugin.hpp"

vector<CSMRRadar *> RadarDisplayOpened;

bool HoppieConnected = false;
bool ConnectionMessage = false;

string logonCode = "";
string logonCallsign = "LFPG";

HttpHelper * httpHelper = NULL;

bool BLINK = false;

bool PlaySoundClr = false;

struct DatalinkPacket {
	string callsign;
	string destination;
	string sid;
	string rwy;
	string freq;
	string ctot;
	string asat;
	string squawk;
	string message;
	string climb;
};

DatalinkPacket DatalinkToSend;

string baseUrlDatalink = "http://www.hoppie.nl/acars/system/connect.html";

struct AcarsMessage {
	string from;
	string type;
	string message;
};

vector<string> AircraftDemandingClearance;
vector<string> AircraftClearanceSent;
vector<string> AircraftWilco;
vector<string> AircraftStandby;
map<string, AcarsMessage> PendingMessages;

string tmessage;
string tdest;

int messageId = 0;

clock_t timer, timer_long;

string maestrourl;

bool startsWith(const char *pre, const char *str)
{
	size_t lenpre = strlen(pre), lenstr = strlen(str);
	return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
};

void datalinkLogin(void * arg) {
	string raw;
	string url = baseUrlDatalink;
	url += "?logon=";
	url += logonCode;
	url += "&from=";
	url += logonCallsign;
	url += "&to=SERVER&type=PING";
	raw.assign(httpHelper->downloadStringFromURL(url));

	if (startsWith("ok", raw.c_str())) {
		HoppieConnected = true;
		ConnectionMessage = true;
	}
};

void sendMaestroData(void * arg) {
	string raw;
	raw.assign(httpHelper->downloadStringFromURL(maestrourl));
};

void sendDatalinkMessage(void * arg) {
	string raw;
	string url = baseUrlDatalink;
	url += "?logon=";
	url += logonCode;
	url += "&from=";
	url += logonCallsign;
	url += "&to=";
	url += tdest;
	url += "&type=CPDLC&packet=/data2/";
	messageId++;
	url += std::to_string(messageId);
	url += "//N/";
	url += tmessage;

	size_t start_pos = 0;
	while ((start_pos = url.find(" ", start_pos)) != std::string::npos) {
		url.replace(start_pos, string(" ").length(), "%20");
		start_pos += string("%20").length();
	}

	raw.assign(httpHelper->downloadStringFromURL(url));
};

void pollMessages(void * arg) {
	string raw = "";
	string url = baseUrlDatalink;
	url += "?logon=";
	url += logonCode;
	url += "&from=";
	url += logonCallsign;
	url += "&to=SERVER&type=POLL";
	raw.assign(httpHelper->downloadStringFromURL(url));

	if (!startsWith("ok", raw.c_str()) || raw.size() <= 3)
		return;

	raw = raw + " ";
	raw = raw.substr(3, raw.size() - 3);

	string delimiter = "}} ";
	size_t pos = 0;
	std::string token;
	while ((pos = raw.find(delimiter)) != std::string::npos) {
		token = raw.substr(1, pos);

		string parsed;
		stringstream input_stringstream(token);
		struct AcarsMessage message;
		int i = 1;
		while (getline(input_stringstream, parsed, ' '))
		{
			if (i == 1)
				message.from = parsed;
			if (i == 2) 
				message.type = parsed;
			if (i > 2)
			{
				message.message.append(" ");
				message.message.append(parsed);
			}
				
			i++;
		}
		if (message.type.find("telex") != std::string::npos || message.type.find("cpdlc") != std::string::npos) {
			if (message.message.find("REQ") != std::string::npos || message.message.find("CLR") != std::string::npos || message.message.find("PDC") != std::string::npos || message.message.find("PREDEP") != std::string::npos || message.message.find("REQUEST") != std::string::npos) {
				if (PlaySoundClr) {
					AFX_MANAGE_STATE(AfxGetStaticModuleState());
						PlaySound(MAKEINTRESOURCE(IDR_WAVE1), AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
				}
				AircraftDemandingClearance.push_back(message.from);
			}
			else if (message.message.find("WILCO") != std::string::npos || message.message.find("ROGER") != std::string::npos || message.message.find("RGR") != std::string::npos) {
				if (std::find(AircraftClearanceSent.begin(), AircraftClearanceSent.end(), message.from) != AircraftClearanceSent.end()) {
					AircraftWilco.push_back(message.from);
				}
			}
			PendingMessages[message.from] = message;
		}

		raw.erase(0, pos + delimiter.length());
	}


};

void sendDatalink(void * arg) {

	string raw;
	string url = baseUrlDatalink;
	url += "?logon=";
	url += logonCode;
	url += "&from=";
	url += logonCallsign;
	url += "&to=";
	url += DatalinkToSend.callsign;
	url += "&type=CPDLC&packet=/data2/";
	messageId++;
	url += std::to_string(messageId);
	url += "//R/";
	url += "CLR TO @";
	url += DatalinkToSend.destination;
	url += "@ RWY @";
	url += DatalinkToSend.rwy;
	url += "@ DEP @";
	url += DatalinkToSend.sid;
	url += "@ INIT CLB @";
	url += DatalinkToSend.climb;
	url += "@ SQUAWK @";
	url += DatalinkToSend.squawk;
	url += "@ ";
	if (DatalinkToSend.ctot != "no" && DatalinkToSend.ctot.size() > 3) {
		url += "CTOT @";
		url += DatalinkToSend.ctot;
		url += "@ ";
	}
	if (DatalinkToSend.asat != "no" && DatalinkToSend.asat.size() > 3) {
		url += "TSAT @";
		url += DatalinkToSend.asat;
		url += "@ ";
	}
	if (DatalinkToSend.freq != "no" && DatalinkToSend.freq.size() > 5) {
		url += "WHEN RDY CALL FREQ @";
		url += DatalinkToSend.freq;
		url += "@";
	}
	else {
		url += "WHEN RDY CALL @VOICE@";
	}
	url += " IF UNABLE CALL VOICE ";
	if (DatalinkToSend.message != "no" && DatalinkToSend.message.size() > 1)
		url += DatalinkToSend.message;
	
	size_t start_pos = 0;
	while ((start_pos = url.find(" ", start_pos)) != std::string::npos) {
		url.replace(start_pos, string(" ").length(), "%20");
		start_pos += string("%20").length();
	}

	raw.assign(httpHelper->downloadStringFromURL(url));

	if (startsWith("ok", raw.c_str())) {
		if (std::find(AircraftDemandingClearance.begin(), AircraftDemandingClearance.end(), DatalinkToSend.callsign.c_str()) != AircraftDemandingClearance.end()) {
			AircraftDemandingClearance.erase(std::remove(AircraftDemandingClearance.begin(), AircraftDemandingClearance.end(), DatalinkToSend.callsign.c_str()), AircraftDemandingClearance.end());
		}
		if (std::find(AircraftStandby.begin(), AircraftStandby.end(), DatalinkToSend.callsign.c_str()) != AircraftStandby.end()) {
			AircraftStandby.erase(std::remove(AircraftStandby.begin(), AircraftStandby.end(), DatalinkToSend.callsign.c_str()), AircraftStandby.end());
		}
		if (PendingMessages.find(DatalinkToSend.callsign) != PendingMessages.end())
			PendingMessages.erase(DatalinkToSend.callsign);
		AircraftClearanceSent.push_back(DatalinkToSend.callsign.c_str());
	}
};

CSMRPlugin::CSMRPlugin(void):CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE, MY_PLUGIN_NAME, MY_PLUGIN_VERSION, MY_PLUGIN_DEVELOPER, MY_PLUGIN_COPYRIGHT)
{
	//
	// Adding the SMR Display type
	//
	RegisterDisplayType(MY_PLUGIN_VIEW_AVISO, false, true, true, true);

	RegisterTagItemType("Datalink clearance", TAG_ITEM_DATALINK_STS);
	RegisterTagItemFunction("Datalink menu", TAG_FUNC_DATALINK_MENU);

	messageId = rand() % 10000 + 1789;

	timer = clock();

	if (httpHelper == NULL)
		httpHelper = new HttpHelper();

	const char * p_value;

	if ((p_value = GetDataFromSettings("cpdlc_logon")) != NULL)
		logonCallsign = p_value;
	if ((p_value = GetDataFromSettings("cpdlc_password")) != NULL)
		logonCode = p_value;
	if ((p_value = GetDataFromSettings("cpdlc_sound")) != NULL)
		PlaySoundClr = bool(atoi(p_value));
}

CSMRPlugin::~CSMRPlugin()
{
	SaveDataToSettings("cpdlc_logon", "The CPDLC logon callsign", logonCallsign.c_str());
	SaveDataToSettings("cpdlc_password", "The CPDLC logon password", logonCode.c_str());
	int temp = 0;
	if (PlaySoundClr)
		temp = 1;
	SaveDataToSettings("cpdlc_sound", "Play sound on clearance request", std::to_string(temp).c_str());
}

bool CSMRPlugin::OnCompileCommand(const char * sCommandLine) {
	if (startsWith(".smr connect", sCommandLine))
	{
		if (ControllerMyself().IsController()) {
			if (!HoppieConnected) {
				_beginthread(datalinkLogin, 0, NULL);
			}
			else {
				HoppieConnected = false;
				DisplayUserMessage("CPDLC", "Server", "Logged off!", true, true, false, true, false);
			}
		}
		else {
			DisplayUserMessage("CPDLC", "Error", "You are not logged in as a controller!", true, true, false, true, false);
		}
		
		return true;
	} else if (startsWith(".smr poll", sCommandLine))
	{
		if (HoppieConnected) {
			_beginthread(pollMessages, 0, NULL);
		}
		return true;
	} else if (startsWith(".smr", sCommandLine))
	{
		CCPDLCSettingsDialog dia;
		dia.m_Logon = logonCallsign.c_str();
		dia.m_Password = logonCode.c_str();
		dia.m_Sound = int(PlaySoundClr);

		if (dia.DoModal() != IDOK)
			return true;

		logonCallsign = dia.m_Logon;
		logonCode = dia.m_Password;
		PlaySoundClr = bool(dia.m_Sound);
		SaveDataToSettings("cpdlc_logon", "The CPDLC logon callsign", logonCallsign.c_str());
		SaveDataToSettings("cpdlc_password", "The CPDLC logon password", logonCode.c_str());
		int temp = 0;
		if (PlaySoundClr)
			temp = 1;
		SaveDataToSettings("cpdlc_sound", "Play sound on clearance request", std::to_string(temp).c_str());

		return true;
	}
	else if (startsWith(".send", sCommandLine)) {
		maestrourl = "http://secure.ferran.io/vMAESTRO/ajax.php?action=plugin&data=[";

		CRadarTarget rt;
		for (rt = RadarTargetSelectFirst();
			rt.IsValid();
			rt = RadarTargetSelectNext(rt))
		{
			if (!rt.IsValid() || !rt.GetPosition().IsValid())
				continue;

			if (rt.GetCorrelatedFlightPlan().IsValid()) {
				CFlightPlan FlightPlan = rt.GetCorrelatedFlightPlan();

				if (startsWith("LFPG", FlightPlan.GetFlightPlanData().GetDestination())) {

					CFlightPlanExtractedRoute Route = FlightPlan.GetExtractedRoute();
					for (int i = 0; i < Route.GetPointsNumber(); ++i) {
						if (startsWith("BANOX", Route.GetPointName(i)) || startsWith("LORNI", Route.GetPointName(i)) || startsWith("OKIPA", Route.GetPointName(i)) || startsWith("MOPAR", Route.GetPointName(i))) {

							if (Route.GetPointDistanceInMinutes(i) == -1)
								break;

							char ch = maestrourl.back();

							if (ch == '}')
								maestrourl += ",";

							maestrourl += "{";

							//{"callsign":"AFR310", "dest": "LFPG", "actype":"A320", "rwy": "26L", "wake":"M", "iaf":"LORNI", "est":1930}

							maestrourl += "'callsign':'" + std::string(FlightPlan.GetCallsign()) + "',";
							maestrourl += "'dest':'" + std::string(FlightPlan.GetFlightPlanData().GetDestination()) + "',";
							maestrourl += "'actype':'" + std::string(FlightPlan.GetFlightPlanData().GetAircraftFPType()) + "',";
							maestrourl += "'rwy':'" + std::string(FlightPlan.GetFlightPlanData().GetArrivalRwy()) + "',";
							string tempS;
							char tempC = FlightPlan.GetFlightPlanData().GetAircraftWtc();
							tempS.push_back(tempC);
							maestrourl += "'wake':'" + std::string(tempS) + "',";
							maestrourl += "'iaf':'" + std::string(Route.GetPointName(i)) + "',";

							long int t = static_cast<long int>(time(NULL));
							t += (Route.GetPointDistanceInMinutes(i) * 60);

							maestrourl += "'est':'" + std::to_string(t) + "'";

							maestrourl += "}";

							break;
						}
					}

				}
			}

		}

		maestrourl += "]";

		DisplayUserMessage("D", "D", maestrourl.c_str(), true, true, true, true, true);

		if (maestrourl != "http://secure.ferran.io/vMAESTRO/ajax.php?action=plugin&data=[]") {
			_beginthread(sendMaestroData, 0, NULL);
		}
	}
	return false;
}

void CSMRPlugin::OnGetTagItem(CFlightPlan FlightPlan, CRadarTarget RadarTarget, int ItemCode, int TagData, char sItemString[16], int * pColorCode, COLORREF * pRGB, double * pFontSize) {
	if (ItemCode == TAG_ITEM_DATALINK_STS) {
		if (FlightPlan.IsValid()) {
			if (std::find(AircraftDemandingClearance.begin(), AircraftDemandingClearance.end(), FlightPlan.GetCallsign()) != AircraftDemandingClearance.end()) {
				*pColorCode = TAG_COLOR_RGB_DEFINED;
				if (BLINK)
					*pRGB = RGB(130, 130, 130);
				else
					*pRGB = RGB(255, 255, 0);

				if (std::find(AircraftStandby.begin(), AircraftStandby.end(), FlightPlan.GetCallsign()) != AircraftStandby.end())
					strcpy_s(sItemString, 16, "S");
				else
					strcpy_s(sItemString, 16, "R");
			}
			else if (std::find(AircraftWilco.begin(), AircraftWilco.end(), FlightPlan.GetCallsign()) != AircraftWilco.end()) {
				*pColorCode = TAG_COLOR_RGB_DEFINED;
				*pRGB = RGB(0, 176, 0);
				strcpy_s(sItemString, 16, "V");
			}
			else if (std::find(AircraftClearanceSent.begin(), AircraftClearanceSent.end(), FlightPlan.GetCallsign()) != AircraftClearanceSent.end()) {
				*pColorCode = TAG_COLOR_RGB_DEFINED;
				*pRGB = RGB(255, 255, 0);
				strcpy_s(sItemString, 16, "V");
			}
			else {
				*pColorCode = TAG_COLOR_RGB_DEFINED;
				*pRGB = RGB(130, 130, 130);

				strcpy_s(sItemString, 16, "N");
			}
		}
	}
}

void CSMRPlugin::OnFunctionCall(int FunctionId, const char * sItemString, POINT Pt, RECT Area)
{
	if (FunctionId == TAG_FUNC_DATALINK_MENU) {
		CFlightPlan FlightPlan = FlightPlanSelectASEL();
		
		bool menu_is_datalink = true;

		if (FlightPlan.IsValid()) {
			if (std::find(AircraftDemandingClearance.begin(), AircraftDemandingClearance.end(), FlightPlan.GetCallsign()) != AircraftDemandingClearance.end()) {
				menu_is_datalink = false;
			}
		}

		OpenPopupList(Area, "Datalink menu", 1);
		AddPopupListElement("Confirm", "", TAG_FUNC_DATALINK_CONFIRM, false, 2, menu_is_datalink);
		AddPopupListElement("Standby", "", TAG_FUNC_DATALINK_STBY, false, 2, menu_is_datalink);
		AddPopupListElement("Voice", "", TAG_FUNC_DATALINK_VOICE, false, 2, menu_is_datalink);
		AddPopupListElement("Reset", "", TAG_FUNC_DATALINK_RESET, false, 2, false, true);
		AddPopupListElement("Close", "", EuroScopePlugIn::TAG_ITEM_FUNCTION_NO, false, 2, false, true);
	}

	if (FunctionId == TAG_FUNC_DATALINK_RESET) {
		CFlightPlan FlightPlan = FlightPlanSelectASEL();

		if (FlightPlan.IsValid()) {
			if (std::find(AircraftDemandingClearance.begin(), AircraftDemandingClearance.end(), FlightPlan.GetCallsign()) != AircraftDemandingClearance.end()) {
				AircraftDemandingClearance.erase(std::remove(AircraftDemandingClearance.begin(), AircraftDemandingClearance.end(), FlightPlan.GetCallsign()), AircraftDemandingClearance.end());
			}
			if (std::find(AircraftStandby.begin(), AircraftStandby.end(), FlightPlan.GetCallsign()) != AircraftStandby.end()) {
				AircraftStandby.erase(std::remove(AircraftStandby.begin(), AircraftStandby.end(), FlightPlan.GetCallsign()), AircraftStandby.end());
			}
			if (std::find(AircraftClearanceSent.begin(), AircraftClearanceSent.end(), FlightPlan.GetCallsign()) != AircraftClearanceSent.end()) {
				AircraftClearanceSent.erase(std::remove(AircraftClearanceSent.begin(), AircraftClearanceSent.end(), FlightPlan.GetCallsign()), AircraftClearanceSent.end());
			}
			if (std::find(AircraftWilco.begin(), AircraftWilco.end(), FlightPlan.GetCallsign()) != AircraftWilco.end()) {
				AircraftWilco.erase(std::remove(AircraftWilco.begin(), AircraftWilco.end(), FlightPlan.GetCallsign()), AircraftWilco.end());
			}
			if (PendingMessages.find(FlightPlan.GetCallsign()) != PendingMessages.end()) {
				PendingMessages.erase(FlightPlan.GetCallsign());
			}
		}
	}

	if (FunctionId == TAG_FUNC_DATALINK_STBY) {
		CFlightPlan FlightPlan = FlightPlanSelectASEL();

		if (FlightPlan.IsValid()) {
			AircraftStandby.push_back(FlightPlan.GetCallsign());
			tmessage = "STANDBY";
			tdest = FlightPlan.GetCallsign();
			_beginthread(sendDatalinkMessage, 0, NULL);
		}
	}

	if (FunctionId == TAG_FUNC_DATALINK_VOICE) {
		CFlightPlan FlightPlan = FlightPlanSelectASEL();

		if (FlightPlan.IsValid()) {
			tmessage = "UNABLE CALL ON FREQ";
			tdest = FlightPlan.GetCallsign();

			if (std::find(AircraftDemandingClearance.begin(), AircraftDemandingClearance.end(), DatalinkToSend.callsign.c_str()) != AircraftDemandingClearance.end()) {
				AircraftDemandingClearance.erase(std::remove(AircraftDemandingClearance.begin(), AircraftDemandingClearance.end(), FlightPlan.GetCallsign()), AircraftDemandingClearance.end());
			}
			if (std::find(AircraftStandby.begin(), AircraftStandby.end(), DatalinkToSend.callsign.c_str()) != AircraftStandby.end()) {
				AircraftStandby.erase(std::remove(AircraftStandby.begin(), AircraftStandby.end(), FlightPlan.GetCallsign()), AircraftDemandingClearance.end());
			}
			PendingMessages.erase(DatalinkToSend.callsign);

			_beginthread(sendDatalinkMessage, 0, NULL);
		}

	}

	if (FunctionId == TAG_FUNC_DATALINK_CONFIRM) {
		CFlightPlan FlightPlan = FlightPlanSelectASEL();

		if (FlightPlan.IsValid()) {

			AFX_MANAGE_STATE(AfxGetStaticModuleState());

				CDataLinkDialog dia;
				dia.m_Callsign = FlightPlan.GetCallsign();
				dia.m_Aircraft = FlightPlan.GetFlightPlanData().GetAircraftFPType();
				dia.m_Dest = FlightPlan.GetFlightPlanData().GetDestination();
				dia.m_From = FlightPlan.GetFlightPlanData().GetOrigin();
				dia.m_Departure = FlightPlan.GetFlightPlanData().GetSidName();
				dia.m_Rwy = FlightPlan.GetFlightPlanData().GetDepartureRwy();
				dia.m_SSR = FlightPlan.GetControllerAssignedData().GetSquawk();
				string freq = std::to_string(ControllerSelect(FlightPlan.GetCoordinatedNextController()).GetPrimaryFrequency());
				freq = freq.substr(0, 6);
				dia.m_Freq = freq.c_str();
				AcarsMessage msg = PendingMessages[FlightPlan.GetCallsign()];
				dia.m_Req = msg.message.c_str();

				string toReturn = "";

				int ClearedAltitude = FlightPlan.GetControllerAssignedData().GetClearedAltitude();
				int Ta = GetTransitionAltitude();

				if (ClearedAltitude != 0) {
					if (ClearedAltitude > Ta && ClearedAltitude > 2) {
						string str = std::to_string(ClearedAltitude);
						for (size_t i = 0; i < 5 - str.length(); i++)
							str = "0" + str;
						if (str.size() > 3)
							str.erase(str.begin() + 3, str.end());
						toReturn = "FL";
						toReturn += str;
					}
					else if (ClearedAltitude <= Ta && ClearedAltitude > 2) {
						

						toReturn = std::to_string(ClearedAltitude);
						toReturn += "ft";
					}
				}
				dia.m_Climb = toReturn.c_str();

				if (dia.DoModal() != IDOK)
					return;

				DatalinkToSend.callsign = FlightPlan.GetCallsign();
				DatalinkToSend.destination = FlightPlan.GetFlightPlanData().GetDestination();
				DatalinkToSend.rwy = FlightPlan.GetFlightPlanData().GetDepartureRwy();
				DatalinkToSend.sid = FlightPlan.GetFlightPlanData().GetSidName();
				DatalinkToSend.asat = dia.m_TSAT;
				DatalinkToSend.ctot = dia.m_CTOT;
				DatalinkToSend.freq = dia.m_Freq;
				DatalinkToSend.message = dia.m_Message;
				DatalinkToSend.squawk = FlightPlan.GetControllerAssignedData().GetSquawk();
				DatalinkToSend.climb = toReturn;

				_beginthread(sendDatalink, 0, NULL);

		}

	}
}

void CSMRPlugin::OnControllerDisconnect(CController Controller) {
	if (Controller.GetFullName() == ControllerMyself().GetFullName()) {
		HoppieConnected = false;
		DisplayUserMessage("CPDLC", "Server", "Logged off!", true, true, false, true, false);
	}
}

void CSMRPlugin::OnTimer(int Counter)
{
	BLINK = !BLINK;

	if (HoppieConnected && ConnectionMessage) {
		DisplayUserMessage("CPDLC", "Server", "Logged in!", true, true, false, true, false);
		ConnectionMessage = false;
	}

	if (((clock() - timer) / CLOCKS_PER_SEC) > 10 && HoppieConnected) {
		_beginthread(pollMessages, 0, NULL);
		timer = clock();
	}

	if (((clock() - timer_long) / CLOCKS_PER_SEC) > 30) {
		
		timer_long = clock();

		maestrourl = "http://secure.ferran.io/vMAESTRO/ajax.php?action=plugin&data=[";

		CRadarTarget rt;
		for (rt = RadarTargetSelectFirst();
			rt.IsValid();
			rt = RadarTargetSelectNext(rt))
		{
			if (!rt.IsValid() || !rt.GetPosition().IsValid())
				continue;

			if (rt.GetCorrelatedFlightPlan().IsValid()) {
				CFlightPlan FlightPlan = rt.GetCorrelatedFlightPlan();

				if (startsWith("LFPG", FlightPlan.GetFlightPlanData().GetDestination())) {

					CFlightPlanExtractedRoute Route = FlightPlan.GetExtractedRoute();
					for (int i = 0; i < Route.GetPointsNumber(); ++i) {
						if (startsWith("BANOX", Route.GetPointName(i)) ||startsWith("LORNI", Route.GetPointName(i)) || startsWith("OKIPA", Route.GetPointName(i)) || startsWith("MOPAR", Route.GetPointName(i))) {
							
							if (Route.GetPointDistanceInMinutes(i) == -1)
								break;

							char ch = maestrourl.back();

							if (ch == '}')
								maestrourl += ",";

							maestrourl += "{";
								
							//{"callsign":"AFR310", "dest": "LFPG", "actype":"A320", "rwy": "26L", "wake":"M", "iaf":"LORNI", "est":1930}

							maestrourl += "'callsign':'" + std::string(FlightPlan.GetCallsign()) + "',";
							maestrourl += "'dest':'" + std::string(FlightPlan.GetFlightPlanData().GetDestination()) + "',";
							maestrourl += "'actype':'" + std::string(FlightPlan.GetFlightPlanData().GetAircraftFPType()) + "',";
							maestrourl += "'rwy':'" + std::string(FlightPlan.GetFlightPlanData().GetArrivalRwy()) + "',";
							string tempS;
							char tempC = FlightPlan.GetFlightPlanData().GetAircraftWtc();
							tempS.push_back(tempC);
							maestrourl += "'wake':'" + std::string(tempS) + "',";
							maestrourl += "'iaf':'" + std::string(Route.GetPointName(i)) + "',";

							long int t = static_cast<long int>(time(NULL));
							t += (Route.GetPointDistanceInMinutes(i) * 60);

							maestrourl += "'est':'" + std::to_string(t) + "'";

							maestrourl += "}";

							break;
						}
					}

				}
			}

		}

		maestrourl += "]";

		if (maestrourl != "http://secure.ferran.io/vMAESTRO/ajax.php?action=plugin&data=[]") {
			_beginthread(sendMaestroData, 0, NULL);
		}
	}

	for (auto &ac : AircraftWilco) // access by reference to avoid copying
	{
		CRadarTarget RadarTarget = RadarTargetSelect(ac.c_str());

		if (RadarTarget.IsValid()) {
			if (RadarTarget.GetGS() > 160) {
				AircraftWilco.erase(std::remove(AircraftWilco.begin(), AircraftWilco.end(), ac), AircraftWilco.end());
			}
		}
	}
};

CRadarScreen * CSMRPlugin::OnRadarScreenCreated(const char * sDisplayName, bool NeedRadarContent, bool GeoReferenced, bool CanBeSaved, bool CanBeCreated)
{
	if (!strcmp(sDisplayName, MY_PLUGIN_VIEW_AVISO)) {
		CSMRRadar * Radar = new CSMRRadar();
		RadarDisplayOpened.push_back(Radar);
		return Radar;
	}

	return NULL;
}
