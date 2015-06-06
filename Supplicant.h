#include <stdlib.h>
#include <iostream>
#include <pcap.h>
#include <string.h>
#include <string>
#include "headers.h"
#include "md5.h"
#include "Logger.h"
#include "./app/Controller.h"
#pragma comment(lib,"ws2_32.lib")

using namespace std;

class Supplicant
{
private:
	pcap_t* fp;
	pcap_if_t *d; 
	char errbuf[PCAP_ERRBUF_SIZE];
	u_char destinationMac[6];
	u_char sourceMac[6];
	string login;
	u_char connectionIdentifier;
	string challenge;
	string password;
	int packetCounter;
	Logger& logger;
	u_char lastIdentifier;
	pcap_if_t *alldevs;
	Controller* controller;
	CriticalSectionPack* connectPack;
	void chooseDevice (const int& inum);
	void lockCase ();
	void destinationMacOnGUI ();
public:
	bool sessionActive;
	Supplicant():logger(Logger::getInstance()){}
	~Supplicant(){}
	
	void init (u_char*, u_char*, Controller*, CriticalSectionPack*);
	int eapolStart();
	int eapolLogoff();
	int eapResponseIdentify();
	int eapResponseChallenge();
	int listen();
	void listenNext();
	void setChallenge(string);
	void setLogin(string);
	void setPassword(string);
	string getDestinationMac();
	string getSourceMac();
};
