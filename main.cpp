#include <iostream>
#include "Supplicant.h"
#include "Config.h"
#include <pcap.h>
#include "./app/Application.h"
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

std::mutex mtx;
std::condition_variable cv;
bool ready = false;


void funkcjathreada (Controller* controller) {
	while (controller->running ()) {
		Config c;
		c.parse_config ("CONFIG.txt");

		pcap_if_t *d, *alldevs;

		char errbuf [PCAP_ERRBUF_SIZE];
		pcap_findalldevs_ex ("rpcap://", NULL, &alldevs, errbuf);
		Supplicant s;

		int i = 19;
		cout << "i = " << hex << (char) i << endl;
		s.init (controller);
			
		std::unique_lock<std::mutex> lck (mtx);
		while (!ready) cv.wait (lck);
		s.eapolStart ();

		controller->sendMessagge ("costam");

		//s.eapResponseIdentify();
		//s.eapResponseChallenge();
		//s.eapolLogoff();

		while (s.sessionActive)
			s.listenNext ();


		cout << "closing connection";
		s.eapolLogoff ();
		cout << "done!" << endl;
	}
}




int main()
{
	Controller controller;
	controller.setCriticalSection (&mtx, &ready, &cv);
	std::thread thr (funkcjathreada, &controller);
	Application app(&controller);
	app.initialize ();
	app.run ();

	

	thr.join ();
	
	return 0;
}
