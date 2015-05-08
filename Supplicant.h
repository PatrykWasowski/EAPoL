#include <stdlib.h>
#include <pcap.h>

using namespace std;

class Supplicant
{
private:
	pcap_t* fp;
	u_char MACAdress[6];
	char errbuf[PCAP_ERRBUF_SIZE];

public:
	Supplicant(){}
	~Supplicant(){}
	
	void init();
	//void setMac(u_char mac [6]);
	//char* getMac();

};