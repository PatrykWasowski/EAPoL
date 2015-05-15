#include <stdlib.h>
#include <iostream>
#include <pcap.h>
#include <string.h>
#include <string>
#include "headers.h"
#include "md5.h"
#include "Logger.h"

#pragma comment(lib,"ws2_32.lib")

using namespace std;

class Supplicant
{
private:
	pcap_t* fp;
	pcap_if_t *d; //d
	char errbuf[PCAP_ERRBUF_SIZE];
	u_char destinationMac[6];
	u_char sourceMac[6];
	char login[100];
	u_char connectionIdentifier;
	char challenge[100];
	char password[100];
	int packetCounter;
	Logger& logger;
	u_char lastIdentifier;
	//bool sessionActive;
public:
	
	bool sessionActive;
	Supplicant():logger(Logger::getInstance()){}
	~Supplicant(){}
	
	void init();
	int eapolStart();
	int eapolLogoff();
	int eapResponseIdentify();
	int eapResponseChallenge();
	int listen();
	void listenNext();
	void setChallenge(char* );
	void setLogin(char*);
	void setPassword(char* );
	string getDestinationMac();
	string getSourceMac();
	string convertToHex(u_char*);
	
	//void setMac(u_char mac [6]);
	//char* getMac();
};
/*
class Listener
{
private:
	static Supplicant& sup;

public:
	Listener(Supplicant& s){}
	~Listener(){}
	void packetListener(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
};*/