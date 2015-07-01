#include "stdafx.h"
#include "MapData.h"


CMapData::CMapData(string configPath)
{
	config_path = configPath;
	loadConfig();
}

void CMapData::loadConfig() {
	Document document;
	stringstream ss;
	ifstream ifs;
	ifs.open(config_path.c_str(), std::ios::binary);
	ss << ifs.rdbuf();
	ifs.close();

	if (document.Parse<0>(ss.str().c_str()).HasParseError())
		throw std::invalid_argument("json parse error");

	custom_maps.clear();

	const Value& root = document;
	for (Value::ConstValueIterator itr = root.Begin(); itr != root.End(); ++itr) {
		const Value& profile = itr;
		string profile_name = profile["icao"].GetString();

		custom_maps.insert(pair<string, const Value&>(profile_name, profile));
	}
}

const Value& CMapData::getActiveMap(string icao) {
	return custom_maps[icao];
}

bool CMapData::isThereAMap(string icao) {
	if (custom_maps.find(icao) != custom_maps.end())
		return true;
	return false;
}

CMapData::~CMapData()
{
}
