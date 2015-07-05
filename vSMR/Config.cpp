#include "stdafx.h"
#include "Config.hpp"

CConfig::CConfig(string configPath)
{
	config_path = configPath;
	loadConfig();

	setActiveProfile("AVISO");
}

void CConfig::loadConfig() {
	
	stringstream ss;
	ifstream ifs;
	ifs.open(config_path.c_str(), std::ios::binary);
	ss << ifs.rdbuf();
	ifs.close();

	if (document.Parse<0>(ss.str().c_str()).HasParseError())
		throw std::invalid_argument("json parse error");

	profiles.clear();

	assert(document.IsArray());

	for (SizeType i = 0; i < document.Size(); i++) {
		const Value& profile = document[i];
		string profile_name = profile["name"].GetString();

		profiles.insert(pair<string, rapidjson::SizeType>(profile_name, i));
	}
}

const Value& CConfig::getActiveProfile() {
	return document[active_profile];
}

Gdiplus::Color CConfig::getConfigColor(const Value& config_path) {
	int r = config_path["r"].GetInt();
	int g = config_path["g"].GetInt();
	int b = config_path["b"].GetInt();
	int a = 255;
	if (config_path.HasMember("a"))
		a = config_path["a"].GetInt();

	Gdiplus::Color Color(a, r, g, b);
	return Color;
}

COLORREF CConfig::getConfigColorRef(const Value& config_path) {
	int r = config_path["r"].GetInt();
	int g = config_path["g"].GetInt();
	int b = config_path["b"].GetInt();

	COLORREF Color(RGB(r, g, b));
	return Color;
}

vector<string> CConfig::getAllProfiles() {
	vector<string> toR;
	for (std::map<string, rapidjson::SizeType>::iterator it = profiles.begin(); it != profiles.end(); ++it)
	{
		toR.push_back(it->first);
	}
	return toR;
}

CConfig::~CConfig()
{
}