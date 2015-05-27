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
//std::mutex mtx;
//std::condition_variable cv;
//bool ready = false;


void funkcjathreada (Controller* controller) {
	Config c;
	c.parse_config ("CONFIG.txt");

	pcap_if_t *d, *alldevs;

	char errbuf [PCAP_ERRBUF_SIZE];
	pcap_findalldevs_ex ("rpcap://", NULL, &alldevs, errbuf);
	Supplicant s;

	int i = 19;
	std::cout << "i = " << hex << (char) i << endl;

	while (controller->running ()) {
		s.init (c.supplicant_mac, controller, &disconnectPack);
		
		controller->setButtonLocked (0, false);
		std::unique_lock<std::mutex> lck (connectPack.mtx);
		while (!connectPack.ready) connectPack.cv.wait (lck);
		if (!controller->running ())
			return;
		controller->setButtonLocked (1, false);
		controller->setButtonLocked (0, true);
		connectPack.ready = false;
		s.eapolStart ();
	
		while (s.sessionActive)
			s.listenNext ();

		cout << "closing connection";
		s.eapolLogoff ();
		
		
		//place to unlock connect button 

		cout << "done!" << endl;
	}
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
