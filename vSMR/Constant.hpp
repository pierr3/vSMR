#pragma once
#include <afxwin.h>
#include "stdafx.h"

const int TAG_ITEM_DATALINK_STS = 444;
const int TAG_FUNC_DATALINK_MENU = 544;

const int TAG_FUNC_DATALINK_CONFIRM = 545;
const int TAG_FUNC_DATALINK_STBY = 546;
const int TAG_FUNC_DATALINK_VOICE = 547;
const int TAG_FUNC_DATALINK_RESET = 548;

static bool startsWith(const char *pre, const char *str)
{
	size_t lenpre = strlen(pre), lenstr = strlen(str);
	return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
};

const int DRAWING_TAG = 1211;
const int DRAWING_AC_SYMBOL = 1212;

const int TAG_CITEM_CALLSIGN = 1911;
const int TAG_CITEM_FPBOX = 1912;
const int TAG_CITEM_RWY = 1913;
const int TAG_CITEM_GATE = 1914;

// RIMCAS Menus & shit
const int RIMCAS_CLOSE = EuroScopePlugIn::TAG_ITEM_FUNCTION_NO;
const int RIMCAS_ACTIVE_AIRPORT = 7999;
const int RIMCAS_ACTIVE_AIRPORT_FUNC = 8008;
const int RIMCAS_DISPLAY_MENU = 8000;
const int RIMCAS_RIMCAS_MENU = 8001;
const int RIMCAS_QDM_TOGGLE = 8006;
const int RIMCAS_CA_MONITOR = 9873;
const int RIMCAS_CA_MONITOR_FUNC = 8022;
const int RIMCAS_CA_ARRIVAL = 9874;
const int RIMCAS_CA_ARRIVAL_FUNC = 8020;
const int RIMCAS_CLOSED_RUNWAYS = 9875;
const int RIMCAS_CLOSED_RUNWAYS_FUNC = 8021;
const int RIMCAS_TAGS_MENU = 8007;
const int RIMCAS_TAGS_2NDLINE = 8009;
const int RIMCAS_APPWINDOW = 8010;
const int RIMCAS_UPDATERANGE = 8011;
const int RIMCAS_UPDATEFILTER = 8012;
const int RIMCAS_PRIMARY = 8013;
const int RIMCAS_TAGCOLOR = 8014;
const int RIMCAS_TIMER = 8015;
const int RIMCAS_TAGS_ACTYPE = 8016;
const int RIMCAS_TAGS_SQWARNING = 8017;
const int RIMCAS_TAGS_SPEEDGATE = 8018;
