#include <iostream>
#include <pcap.h>
#include "Server.h"
using namespace std;



void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);


int main()
{
	cout << "server hello" << endl;
	pcap_if_t *d, *alldevs;

	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_findalldevs_ex("rpcap://", NULL, &alldevs, errbuf);

	Server server;
	server.init();

	return 0;

}