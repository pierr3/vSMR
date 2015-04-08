#include "stdafx.h"
#include "CallsignLookup.hpp"

//
// CCallsignLookup Class by Even Rognlien, used with permission
//

CCallsignLookup::CCallsignLookup(std::string fileName) {


	ifstream myfile;

	myfile.open(fileName);

	if (myfile) {
		string line;
		stringstream sstream;

		while (getline(myfile, line)) {
			sstream << line;
			string key;
			string value;

			std::string delimiter = "\t";

			std::string token;

			size_t pos1 = line.find(delimiter);
			key = line.substr(0, pos1);
			line.erase(0, pos1 + delimiter.length());

			size_t pos2 = line.find(delimiter);
			line.erase(0, pos2 + delimiter.length());
			value = line.substr(0, line.length());

			for (unsigned int i = 0; i < value.size(); i++)
				value[i] = toupper(value[i]);

			callsigns[key] = value;
		}
	}

	myfile.close();
}

string CCallsignLookup::getCallsign(string airlineCode) {

	if (callsigns.find(airlineCode) == callsigns.end())
		return "";

	return callsigns.find(airlineCode)->second;
}

CCallsignLookup::~CCallsignLookup()
{
}