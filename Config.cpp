#include "Config.h"

int Config::parse_config(string path){
	ifstream config_file(path);

	std::string str;
	while (getline(config_file, str))
	{
		if (str[0] == '#'){
			continue;
		}

		std::istringstream buf(str);
		std::istream_iterator<std::string> beg(buf), end;

		vector<string> tokens(beg, end); 

		for (auto& s : tokens){
			if (tokens[1] != "="){
				cout << "bledny plik conf.\n";
				return 1;
			}
		

			if (tokens[0] == "connection_mode"){
				istringstream iss(tokens[2]);
				iss >> connection_mode;
			}
			else if(tokens[0] == "server_mac"){
				vector<string> mac_tokens;
				split(tokens[2], ':', mac_tokens);
				for (int i = 0; i < 6; i++){
					istringstream iss(mac_tokens[i]);
					iss >> server_mac[i];
				}
			}
			else if (tokens[0] == "supplicant_mac"){
				vector<string> mac_tokens;
				split(tokens[2], ':', mac_tokens);
				for (int i = 0; i < 6; i++){
					istringstream iss(mac_tokens[i]);
					iss >>supplicant_mac[i];
				}
			}
			else{
				continue;
			}
		}
			
	}

	cout << "Loading config file - Done!\n";
	cout << "\tconnection_mode: " << connection_mode<<endl;
	cout << "\tserver_mac: ";
	for (int i = 0; i < 6; i++){
		printf("%c", server_mac[i]);
		if (i < 5) printf(":");
	}
	cout << "\n\tsupplicant_mac: ";
	for (int i = 0; i < 6; i++){
		printf("%.2X", supplicant_mac[i]);
		if (i < 5) printf(":");
	}
	cout << endl << endl;

	return 0;
}

vector<string>& Config::split(const string &s, char delim, vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}