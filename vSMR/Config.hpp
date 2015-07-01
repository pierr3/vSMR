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

	void setProfile(string profile_name);
	const Value& getActiveProfile();

	Gdiplus::Color getConfigColor(const Value& config_path);
	COLORREF getConfigColorRef(const Value& config_path);

	vector<string> getAllProfiles();

	inline int isItActiveProfile(string toTest) {
		if (active_profile == toTest)
			return 1;
		return 0;
	};

	inline void setActiveProfile(string newProfile) {
		active_profile = newProfile;
	}

protected:
	string config_path;
	string active_profile;
	map<string, const Value&> profiles;

	void loadConfig();
};
