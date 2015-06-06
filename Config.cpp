#include "Config.h"

Config::Config(){
	interface_number = 1;
	for (int i = 0; i < 6; i++){
		supplicant_mac[i] = 1;
		server_mac[i] = 1;
	}
}


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
		

			if (tokens[0] == "interface_number"){
				istringstream iss(tokens[2]);
				iss >> interface_number;
			}
			else if(tokens[0] == "server_mac"){
				vector<string> mac_tokens;
				split(tokens[2], ':', mac_tokens);
				for (int i = 0; i < 6; i++){
					int frag;
					istringstream iss(mac_tokens[i]);
					iss >> std::hex >> frag;
					server_mac[i] = frag;
				}
			}
			else if (tokens[0] == "supplicant_mac"){
				vector<string> mac_tokens;
				split(tokens[2], ':', mac_tokens);
				for (int i = 0; i < 6; i++){
					istringstream iss(mac_tokens[i]);
					int frag;
					iss >> std::hex >> frag;
					supplicant_mac[i] = frag;
				}
			}
			else{
				continue;
			}
		}
			
	}
	return 0;
}
void Config::save_config(string path){
	ofstream f;
	f.open(path, ios::out | ios::trunc);
	f << "#Plik z ostanimi ustawieniami aplikacji. Generowany automatycznie - nie nalezy do niego nic dopisywac, jedynie edytowac." << endl;
	f << "#Przy edycji nalezy zachowac jego format, tj. spacje, dwukropki. " << endl;
	f << "interface_number = " << interface_number << endl;
	f << "server_mac = ";
	for (int i = 0; i < 6; i++){
		f << std::hex <<(int) server_mac[i];
		if (i < 5) 
			f << ":";
		else 
			f << endl;
	}
	
	f << "supplicant_mac = ";
	for (int i = 0; i < 6; i++){
		f << std::hex << (int)supplicant_mac[i];
		if (i < 5) 
			f << ":";
		else  
			f << endl;
	}	
	cout << "Saving config file - Done!\n";
}

vector<string>& Config::split(const string &s, char delim, vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}