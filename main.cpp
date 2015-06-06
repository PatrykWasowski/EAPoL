#include <iostream>
#include "Supplicant.h"
#include "Config.h"
#include <pcap.h>
#include "./app/Application.h"
#include <thread>
#include "CriticalSectionPack.h"
using namespace std;


CriticalSectionPack connectPack;
CriticalSectionPack disconnectPack;

void funkcjathreada (Controller* controller) {
	Config c;
	c.parse_config ("CONFIG.txt");
	controller->setLastPort (c.interface_number);
	pcap_if_t *alldevs;

	char errbuf [PCAP_ERRBUF_SIZE];
	pcap_findalldevs_ex ("rpcap://", NULL, &alldevs, errbuf);
	Supplicant s;

	int i = 19;
	std::cout << "i = " << hex << (char) i << endl;

	while (controller->running ()) {
		s.init (c.supplicant_mac, c.server_mac, controller, &disconnectPack);
		
		controller->setButtonLocked (0, false);
		std::unique_lock<std::mutex> lck (connectPack.mtx);
		while (!connectPack.ready) connectPack.cv.wait (lck);
		if (!controller->running ())
			break;
		controller->setButtonLocked (1, false);
		controller->setButtonLocked (0, true);
		connectPack.ready = false;
		s.eapolStart ();
	
		while (s.sessionActive)
			s.listenNext ();
		controller->setButtonLocked (1, true);
	
		//place to unlock connect button 
	}
	c.interface_number = controller->getLastPort ();
	c.save_config ("CONFIG.TXT");
}


int main () {
	Controller controller;
	controller.setCriticalSection (&connectPack, &disconnectPack);
	std::thread thr (funkcjathreada, &controller);
	Application app (&controller);
	app.initialize ();
	app.run ();

	thr.join ();
	return 0;
}
