#include "Supplicant.h"

using namespace std;

void Supplicant::init(u_char* supadd, u_char* servadd, Controller* ctr, CriticalSectionPack* csp)
{
	
	controller = ctr;
	connectPack = csp;
	int i = 0;
	char errbuf[PCAP_ERRBUF_SIZE];

	
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	controller->clearOptions ();

	for (d = alldevs; d; d = d->next)
	{
		controller->addOption (d->description);
		++i;
	}
	controller->selectLastPort ();
	if (i == 0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		return;
	}

	for (int i = 0; i < 6; ++i)
		sourceMac[i] = (u_char)supadd[i];
	
	sessionActive = 1;
	packetCounter = 0;
	lastIdentifier = (u_char)0x40;
	/*
	sourceMac [0] = 0xe8;
	sourceMac [1] = 0x9a;
	sourceMac [2] = 0x8f;
	sourceMac [3] = 0x88;
	sourceMac [4] = 0x4f;
	sourceMac [5] = 0x36;
*/
	
	//for (int i = 0; i < 6; ++i)
	//	destinationMac [i] = (u_char) servadd [i];
	
	
	destinationMac [0] = 0x01;
	destinationMac [1] = 0x80;
	destinationMac [2] = 0xc2;
	destinationMac [3] = 0x00;
	destinationMac [4] = 0x00;
	destinationMac [5] = 0x03;

	destinationMacOnGUI ();

	
	/*sourceMac [0] = 0x01;
	sourceMac [1] = 0x80;
	sourceMac [2] = 0xc2;
	sourceMac [3] = 0x00;
	sourceMac [4] = 0x00;
	sourceMac [5] = 0x03;*/


}

void Supplicant::chooseDevice (const int& inum) {
	int i;
	for (d = alldevs,  i = 0; i < inum - 1; d = d->next, i++);

	if ((fp = pcap_open (d->name,    // name of the device
		100,						// portion of the packet to capture (only the first 100 bytes)
		PCAP_OPENFLAG_PROMISCUOUS,  // promiscuous mode
		1000,						// read timeout
		NULL,						// authentication on the remote machine
		errbuf						// error buffer
		)) == NULL)
		return;
}


void Supplicant::setChallenge(string cha)
{
	challenge = cha;
}

void Supplicant::setLogin(string log)
{
	login = log;
}

void Supplicant::setPassword(string pas)
{
	password = pas;
}

int Supplicant::eapolStart()
{

	std::vector<string> vct;
	vct = controller->getData ();

	setPassword (vct [1]);
	setLogin (vct [0]);


	chooseDevice (controller->getOption ());



	string log("");
	log += getDestinationMac();
	log += " ";
	log += getSourceMac();
	log += " ";
	cout << "Sending packet:" << endl;
	
	u_char packet_buffer[100];
	ETHERNET_HEADER* eth;

	eth = (ETHERNET_HEADER*)packet_buffer;
	memcpy(eth->destination, destinationMac, 6);
	memcpy(eth->source, sourceMac, 6);
	eth->type = htons(0x888E);
	eth->protocol_version = 2;
	eth->packet_type = 1;
	eth->packet_body_length = 0;

	if (pcap_sendpacket(fp, packet_buffer, sizeof(ETHERNET_HEADER)) != 0)
	{
		fprintf(stderr, "\nError sending the EAPOL-Start packet: \n", pcap_geterr(fp));
		return 1;
	}
	log += "Packet sent: EAPOL-Packet, Start\n";
	logger << log;
	controller->sendMessagge ("Sent packet EAPOL-Start");
	return 0;
}


int Supplicant::eapolLogoff()
{
	string log("");
	log += getDestinationMac();
	log += " ";
	log += getSourceMac();
	log += " ";

	u_char packet_buffer[100];
	ETHERNET_HEADER* eth;

	eth = (ETHERNET_HEADER*)packet_buffer;
	memcpy(eth->destination, destinationMac, 6);
	memcpy(eth->source, sourceMac, 6);
	eth->type = htons(0x888E);
	eth->protocol_version = 2;
	eth->packet_type = 2;
	eth->packet_body_length = 0;

	if (pcap_sendpacket(fp, packet_buffer, sizeof(ETHERNET_HEADER)) != 0)
	{
		fprintf(stderr, "\nError sending the EAPOL-Start packet: \n", pcap_geterr(fp));
		return 1;
	}

	log += "Packet sent: EAPOL-Packet, Logoff\n";
	logger << log;

	controller->sendMessagge ("Sent packet  EAPOL-Logoff");
	return 0;
}



int Supplicant::eapResponseIdentify()
{
	string log("");
	log += getDestinationMac();
	log += " ";
	log += getSourceMac();
	log += " ";
	
	
	u_char packet_buffer[100];
	ETHERNET_HEADER* eth;
	EAP_HEADER* eap;

	eth = (ETHERNET_HEADER*)packet_buffer;
	memcpy(eth->destination, destinationMac, 6);
	memcpy(eth->source, sourceMac, 6);
	eth->type = htons(0x888E);
	eth->protocol_version = 2;
	eth->packet_type = 0;
	eth->packet_body_length = htons(sizeof(EAP_HEADER) + login.length() + 1);

	eap = (EAP_HEADER*)(packet_buffer + sizeof(ETHERNET_HEADER));
	eap->code = 2;
	eap->identifier = lastIdentifier;
	eap->length = htons(sizeof(EAP_HEADER)+1+login.length());

	char* data;
	data = (char *)(packet_buffer + sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER));
	*data = 0x1;
	data = (char*)(packet_buffer + sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER) + 1);
	//strcpy_s(data, login.length()+1 , login.c_str());
	for (int i = 0; i < (int)login.length(); ++i)
		*(data + i) = login[i];

	if (pcap_sendpacket(fp, packet_buffer, sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER) + login.length() + 1) != 0)
	{
		fprintf(stderr, "\nError sending the EAPOL-Start packet: \n", pcap_geterr(fp));
		return 1;
	}

	log += "Packet sent: EAP-Packet, Response/Identify\n";
	logger << log;

	controller->sendMessagge ("Sent packet EAP Response-Identify");
	return 0;
}

int Supplicant::eapResponseChallenge()
{

	string log("");
	log += getDestinationMac();
	log += " ";
	log += getSourceMac();
	log += " "; 
	

	string res = (char)lastIdentifier + password + challenge;

	const char *res2 = res.c_str();
	MD5 md5;
	md5.add(res2, res.length());

	ETHERNET_HEADER* eth;
	EAP_HEADER* eap;
	u_char packet_buffer[100];

	eth = (ETHERNET_HEADER*)packet_buffer;
	memcpy(eth->destination, destinationMac, 6);
	memcpy(eth->source, sourceMac, 6);
	eth->type = htons(0x888E);
	eth->protocol_version = 2;
	eth->packet_type = 0;
	eth->packet_body_length = htons ( sizeof(EAP_HEADER) + 18 + login.length()); // 1 - type , 16 - skrot md5

	eap = (EAP_HEADER*)(packet_buffer + sizeof(ETHERNET_HEADER));
	eap->code = 2;
	eap->identifier = lastIdentifier;
	eap->length =htons( (sizeof(EAP_HEADER) + 18 + login.length() ));
	
	char *data;
	data = (char *)(packet_buffer + sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER));
	*data = 0x4;
	data = (char *)(packet_buffer + sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER) + 1);
	int valueSize = 16 + login.length();
	*data = valueSize;
	++data;
	
	u_char temp[17];
	md5.getHash(temp);
	for (int i = 0; i < 16; ++i)
	{
		*(data + i) = temp[i];
	}
	
	data = data + 16;
	for (int i = 0; i < (int)login.length(); ++i)
		*(data + i) = login[i];

	if (pcap_sendpacket(fp, packet_buffer, sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER) + 18 + login.length()) != 0)
	{
		fprintf(stderr, "\nError sending the EAP-MD5-Challenge Response packet: \n", pcap_geterr(fp));
		return 1;
	}

	log += "Packet sent: EAP-Packet, Response/MD5-Challenge\n";
	logger << log;

	controller->sendMessagge ("Sent packet EAP Response-MD5-Challenge");
	return 0;
}

int Supplicant::listen()
{
	char packet_filter[] = "ether proto 0x888E";	// filtr dopuszczajacy tylko eapol
	struct bpf_program fcode;


	u_int netmask;
	if (d->addresses != NULL)
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		netmask = 0xffffff;

	if (pcap_compile(fp, &fcode, packet_filter, 1, netmask) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		return -1;
	}

	if (pcap_setfilter(fp, &fcode) < 0)
	{
		fprintf(stderr, "\nError setting the filter.\n");
		return -1;
	}

	std::string str = "Listening on ";
	str += d->description;
	controller->sendMessagge (str);	
	return 0;
}

void Supplicant::listenNext()
{
	bool breaker = true;
	char packet_filter[] = "ether proto 0x888E";	
	struct bpf_program fcode;

	u_int netmask;
	if (d->addresses != NULL)
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		netmask = 0xffffff;

	if (pcap_compile(fp, &fcode, packet_filter, 1, netmask) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		return;
	}

	if (pcap_setfilter(fp, &fcode) < 0)
	{
		fprintf(stderr, "\nError setting the filter.\n");
		return;
	}
	
	struct pcap_pkthdr *header; 
	const u_char *pkt_data;

	while (breaker)
	{
		if (!controller->getConnecting ()) {
			breaker = 0;
			sessionActive = 0;
			controller->setButtonLocked (1, true);
			controller->sendMessagge ("Connecting aborted");
			break;
		}
		int res;
		res = pcap_next_ex(fp, &header, &pkt_data);
		u_char temp[100];

		if (res == 0)
			continue;

		++packetCounter;
		string log("");
		log += getDestinationMac();
		log += " ";
		log += getSourceMac();
		log += " ";

		int len = header->len;
		log += "Packet Received:"; 

		for (int i = 0; i < len; ++i)
		{
			temp[i] = *(pkt_data + i);
		}
		cout << endl;

		ETHERNET_HEADER* eth;
		EAP_HEADER* eap;
		char* data;
		eth = (ETHERNET_HEADER*)temp;
		
		switch (eth->packet_type)
		{
			case 0x00:
				log += " EAP-Packet,";
				eap = (EAP_HEADER*)(temp + sizeof(ETHERNET_HEADER));

				lastIdentifier = eap->identifier;

				switch (eap->code)
				{
					case 0x01:
						log += " Request/";
						data = (char*)(temp + sizeof(ETHERNET_HEADER)+sizeof(EAP_HEADER));
						if (*data == (char)0x1)
						{
							log += "Identify\n";
							data = (char*)temp;
							for (int i = 0; i < 6; ++i)
								destinationMac[i] = (u_char)*(data + i);
							destinationMacOnGUI ();
							logger << log;

							controller->sendMessagge ("Received packet EAP Request-Identify");
							eapResponseIdentify();
							breaker = 0;
						}

						if (*data == (char)0x4)
						{
							log += "MD5-Challenge\n";
							data = (char*)(temp + sizeof(ETHERNET_HEADER)+sizeof(EAP_HEADER)+1);
							int valueSize = *data;
							++data;
							string receivedChallenge("");
							for (int i = 0; i < valueSize; ++i)
							{
								receivedChallenge += (char)*(data + i);
							}
							setChallenge(receivedChallenge);

							logger << log;

							controller->sendMessagge ("Received packet EAP Request-MD5-Challenge");
							eapResponseChallenge(); 
							breaker = 0;
						}

					break;
				
					case 0x02:
					//illegal packet
					break;

					case 0x03:
						log += "\n";
						logger << log;


						controller->sendMessagge ("Received packet EAP Success");
						log += " success";
						sessionActive = 0;
						breaker = 0;
						controller->setConnection (true);
						lockCase ();
						controller->setButtonLocked (1, true);
						eapolLogoff ();
					break;

					case 0x04:
						log += " fail";
						log += "\n";
						logger << log;
						controller->sendMessagge ("Received packet EAP Fail");
						breaker = 0;
						sessionActive = 0;
					break;
				
					default:
						controller->sendMessagge ("Received unknown EAP code");
						log += " uknown EAP code";
						logger << log;
				}
			break;
		
			case 0x01:
			log += " EAPOL-Start";
			break;
		
			case 0x02:
			log += " EAPOL-Logoff";
			break;
		
			default:
			log += " unknown type";
		}
	}
}

string Supplicant::getDestinationMac()
{
	string addr;
	addr = "";

	for (int i = 0; i<6; ++i){

		char buffer[50];
		sprintf_s(buffer, "%.2X", destinationMac[i]);
		string str(buffer);
		addr += str;
		if (i != 5)
			addr += ":";
	}

	return addr;
}

string Supplicant::getSourceMac()
{
	string addr;
	addr = "";

	for (int i = 0; i<6; ++i){

		char buffer[50];
		sprintf_s(buffer, "%.2X", sourceMac[i]);
		string str(buffer);
		addr += str;
		if (i != 5)
			addr += ":";
	}

	return addr;
}


void Supplicant::lockCase () {
	std::unique_lock<std::mutex> lck (connectPack->mtx);
	while (!connectPack->ready) connectPack->cv.wait (lck);
	connectPack->ready = false;
}

void Supplicant::destinationMacOnGUI () {
	string addr;
	addr = "";
	for (int i = 0; i<6; ++i) {
		char buffer [50];
		sprintf_s (buffer, "%.2X", destinationMac [i]);
		string str (buffer);
		addr += str;
	}

	controller->setMacAddress (addr);
}