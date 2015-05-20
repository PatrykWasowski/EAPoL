// Suplicant 
//Authors: Maciej Lotz, Szymon Mysiak, Patryk Wasowski, Andrzej Siadkowski

#define VER "Alpha build 0.1"
#define AUTH "Maciej Lotz, Szymon Mysiak, Patryk Wasowski, Andrzej Siadkowski"

#include <iostream>
#include "Supplicant.h"
#include "Config.h"
#include <pcap.h>

using namespace std;


int main()
{
	cout << "Suplicant client - version:" << VER << endl ;
	cout <<"Authors:"<< AUTH << endl << endl;
	Config c;
	c.parse_config("CONFIG.txt");


	
	pcap_if_t *d, *alldevs;

	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_findalldevs_ex("rpcap://", NULL, &alldevs, errbuf);
	Supplicant s;
	
	//int i = 19;
	//cout << "i = " << hex << (char)i << endl;


	s.init();
	s.eapolStart();
	//s.eapResponseIdentify();
	//s.eapResponseChallenge();
	//s.eapolLogoff();
	while (s.sessionActive)
	s.listenNext();


	cout << "Closing connection:"<<endl ;
	s.eapolLogoff();
	cout << "done!"<<endl;
	system("Pause");
	return 0;
}
