#include <stdlib.h>
#include <iostream>
#include <pcap.h>
#include <string.h>
#include <string>
#include "headers.h"
#include "md5.h"

#pragma comment(lib,"ws2_32.lib")

using namespace std;

class Supplicant
{
private:
	
	pcap_if_t *d; //d
	char errbuf[PCAP_ERRBUF_SIZE];
	u_char destinationMac[6];
	u_char sourceMac[6];
	char *login;
	u_char connectionIdentifier;
	char *challenge;
	char *password;

public:
	pcap_t* fp;

	Supplicant(){}
	~Supplicant(){}
	
	void init();
	int eapolStart();
	int eapolLogoff();
	int eapResponseIdentify();
	int eapResponseChallenge();
	int listen();
	void listenNext();
	
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