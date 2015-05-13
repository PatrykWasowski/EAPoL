#include <iostream>
#include "Supplicant.h"
#include <pcap.h>

using namespace std;

int main()
{
	cout << "Hello world" << endl;
	pcap_if_t *d, *alldevs;

	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_findalldevs_ex("rpcap://", NULL, &alldevs, errbuf);
	Supplicant s;
	
	s.init();
	s.eapolStart();
	s.eapResponseIdentify();
	s.eapResponseChallenge();
	//s.eapolLogoff();
	while (s.sessionActive)
		s.listenNext();


	cout << "closing connection" ;
	s.eapolLogoff();
	cout << "done!"<<endl;
	system("Pause");
	return 0;
}