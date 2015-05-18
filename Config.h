#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include "pcap.h"

using namespace std;
class Config
{
public:
	int parse_config(string path);

	unsigned int connection_mode;
	u_char supplicant_mac[6];
	u_char server_mac[6];

	vector<string>& split(const string &s, char delim, vector<string> &elems);
};

