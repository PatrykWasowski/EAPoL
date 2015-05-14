#include <iostream>
#include <pcap.h>
#include "Server.h"
using namespace std;


int main()
{

	for (int i = 0; i < 6; ++i)
	{
		SERVER_MAC[i] = 2;
	}



	cout << "server hello" << endl;
	cout << strlen(CHALLENGE_STRING)<<endl;
	pcap_if_t *alldevs;

	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_findalldevs_ex("rpcap://", NULL, &alldevs, errbuf);

	init();

	return 0;

}