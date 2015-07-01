#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using namespace rapidjson;

class CMapData
{
public:
	CMapData(string configPath);
	virtual ~CMapData();

	const Value& getActiveMap(string icao);
	bool isThereAMap(string icao);

protected:
	string config_path;
	map<string, const Value&> custom_maps;

	void loadConfig();
};

