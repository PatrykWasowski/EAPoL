#include <iostream>
#include "Supplicant.h"
#include "Config.h"
#include <pcap.h>
#include "./app/Application.h"

using namespace std;

int main()
{
	Application app;
	app.initialize ();
	app.run ();



	Config c;
	c.parse_config("CONFIG.txt");


	cout << "Hello world" << endl;
	pcap_if_t *d, *alldevs;

	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_findalldevs_ex("rpcap://", NULL, &alldevs, errbuf);
	Supplicant s;
	
	int i = 19;
	cout << "i = " << hex << (char)i << endl;
	s.init();
	s.eapolStart();
	//s.eapResponseIdentify();
	//s.eapResponseChallenge();
	//s.eapolLogoff();
	while (s.sessionActive)
	s.listenNext();


	cout << "closing connection" ;
	//s.eapolLogoff();
	cout << "done!"<<endl;
	system("Pause");
	return 0;
}
