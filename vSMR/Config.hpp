#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <Gdiplus.h>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Constant.hpp"

using namespace std;
using namespace rapidjson;

class CConfig
{
public:
	CConfig(string configPath);
	virtual ~CConfig();

	const Value& getActiveProfile();
	bool isSidColorAvail(string sid, string airport);
	Gdiplus::Color getSidColor(string sid, string airport);
	const Value& getAirportMapIfAny(string airport);
	bool isAirportMapAvail(string airport);
	bool isCustomRunwayAvail(string airport, string name1, string name2);
	bool isCustomCursorUsed();

	Gdiplus::Color getConfigColor(const Value& config_path);
	COLORREF getConfigColorRef(const Value& config_path);

	vector<string> getAllProfiles();

	inline int isItActiveProfile(string toTest) {
		if (active_profile == profiles[toTest])
			return 1;
		return 0;
	};

	inline void setActiveProfile(string newProfile) {
		active_profile = profiles[newProfile];
	};

	inline string getActiveProfileName() {
		string name;
		for (std::map<string, rapidjson::SizeType>::iterator it = profiles.begin(); it != profiles.end(); ++it)
		{
			if (it->second == active_profile) {
				name = it->first;
				break;
			}
		}
		return name;
	};

	Document document;

protected:
	string config_path;
	rapidjson::SizeType active_profile;
	map<string, rapidjson::SizeType> profiles;

	void loadConfig();
};
