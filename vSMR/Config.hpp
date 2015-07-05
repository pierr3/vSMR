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

using namespace std;
using namespace rapidjson;

class CConfig
{
public:
	CConfig(string configPath);
	virtual ~CConfig();

	const Value& getActiveProfile();

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

	Document document;

protected:
	string config_path;
	rapidjson::SizeType active_profile;
	map<string, rapidjson::SizeType> profiles;

	void loadConfig();
};
