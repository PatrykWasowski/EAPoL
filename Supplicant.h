#include <stdlib.h>
#include <iostream>
#include <pcap.h>
#include <string.h>
#include "headers.h"

#pragma comment(lib,"ws2_32.lib")

using namespace std;

class Supplicant
{
private:
	pcap_t* fp;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_char destinationMac[6];
	u_char sourceMac[6];
	char *login;
	u_char connectionIdentifier;

public:
	Supplicant(){}
	~Supplicant(){}
	
	void init();
	int eapolStart();
	int eapResponseIdentify();
	//void setMac(u_char mac [6]);
	//char* getMac();

};