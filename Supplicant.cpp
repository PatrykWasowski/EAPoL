#include "Supplicant.h"

using namespace std;

void Supplicant::init(u_char* supadd, Controller* ctr)
{
	controller = ctr;
	int inum;
	int i = 0;
	char errbuf[PCAP_ERRBUF_SIZE];

	cout << "List of interfaces:" << endl;

	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	controller->clearOptions ();

	for (d = alldevs; d; d = d->next)
	{
		controller->addOption (d->description);
		printf("%d. %s", ++i, d->name);
		if (d->description)
			printf(" (%s)\n", d->description);
		else
			printf(" (No description available)\n");
	}

	if (i == 0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		return;
	}

	printf("Enter the interface number (1-%d):", i);

	for (int i = 0; i < 6; ++i)
		sourceMac[i] = (u_char)supadd[i];
	
	sessionActive = 1;
	packetCounter = 0;
	lastIdentifier = (u_char)0x40;
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
	cout << "LOG: " << log ;
	logger << log;

	return 0;
}


int Supplicant::eapolLogoff()
{
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
	eth->packet_type = 2;
	eth->packet_body_length = 0;

	if (pcap_sendpacket(fp, packet_buffer, sizeof(ETHERNET_HEADER)) != 0)
	{
		fprintf(stderr, "\nError sending the EAPOL-Start packet: \n", pcap_geterr(fp));
		return 1;
	}

	log += "Packet sent: EAPOL-Packet, Logoff\n";
	cout << "LOG: " << log << endl;
	logger << log;

	return 0;
}



int Supplicant::eapResponseIdentify()
{
	string log("");
	log += getSourceMac();
	log += " ";
	log += getDestinationMac();
	log += " ";
	cout << "Sending packet:" << endl;

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
	cout << "LOG: " << log ;
	logger << log;

	return 0;
}

int Supplicant::eapResponseChallenge()
{

	string log("");
	log += getSourceMac();
	log += " "; 
	log += getDestinationMac();
	log += " ";
	cout << "Sending packet:" << endl;

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
	cout << "LOG: " << log;
	logger << log;

	return 0;
}

int Supplicant::listen()
{
	char packet_filter[] = "ether proto 0x888E";	// filtr dopuszczajacy tylko eapol
	struct bpf_program fcode;

	cout << "!!";

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

	printf("\nlistening on %s...\n", d->description);
	
	return 0;
}

void Supplicant::listenNext()
{
	cout << "Listening for response..." << endl<< endl;
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
		cout << "Packet Received: (nr:" << packetCounter << "), length:" << len ;
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
							data = (char*)(temp + 6);
							for (int i = 0; i < 6; ++i)
								destinationMac[i] = (u_char)*(data + i);
						
							cout << "LOG: " << log ;
							logger << log;

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

							cout << "LOG: " << log ;
							logger << log;

							eapResponseChallenge(); 
							breaker = 0;
						}

					break;
				
					case 0x02:
					//illegal packet
					break;

					case 0x03:
						log += "\n";
						cout << "LOG: " << log;
						logger << log;


						cout << "Success: Client authentication accepted. Access to network granted. " << endl;
						log += " success";
						cout << ".\n.\n." << endl;
						// TU MA SIE ZABLOKOWAC NA MUTEXIE I ODBLOKOWAĆ PO DISCONECT

						 sessionActive = 0;
						 breaker = 0;
					break;

					case 0x04:
						log += " fail";
						log += "\n";
						cout << "LOG: " << log;
						logger << log;
						cout << "!!" << endl;
						cout << "Fail: Client authentication FAILED: Received negative response from RADIUS. " << endl;
				
						breaker = 0;
						sessionActive = 0;
					break;
				
					default:
						cout << "code: def" << endl;
						log += " uknown EAP code";
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


