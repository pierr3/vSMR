#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>

using namespace std;

class CCallsignLookup
{
private:
	std::map<string, string> callsigns;


public:

	CCallsignLookup();
	void readFile(string fileName);
	string getCallsign(string airlineCode);

	~CCallsignLookup();
};