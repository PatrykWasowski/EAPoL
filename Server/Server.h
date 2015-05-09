#pragma once
#include <pcap.h>
class Server
{
public:
	Server();
	~Server();

	int init();
private:
	static void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

};

