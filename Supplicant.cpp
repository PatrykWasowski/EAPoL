#include "Supplicant.h"

using namespace std;

void Supplicant::init()
{

	pcap_if_t *alldevs;
	//pcap_if_t *d;
	int inum;
	int i = 0;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];

	/* Retrieve the device list on the local machine */
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	/* Print the list */
	for (d = alldevs; d; d = d->next)
	{
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
	scanf_s("%d", &inum);

	if (inum < 1 || inum > i)
	{
		printf("\nInterface number out of range.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return;
	}

	/* Jump to the selected adapter */
	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);


	if ((fp = pcap_open(d->name,    // name of the device
		100,						// portion of the packet to capture (only the first 100 bytes)
		PCAP_OPENFLAG_PROMISCUOUS,  // promiscuous mode
		1000,						// read timeout
		NULL,						// authentication on the remote machine
		errbuf						// error buffer
		)) == NULL)
		;

	for (int i = 0; i < 6; ++i)
	{
		destinationMac[i] = 1;
		sourceMac[i] = 1;
	}

	connectionIdentifier = 0x0;		// TODO: ARAP MUSI WYZNACZYC, A MY MUSIMY TO ODEBRAC!

	//login = "Maciek";
	//password = "pass";
	//challenge = "chal";
	setChallenge("chal");
	setLogin("Maciek");
	setPassword("pass");
	sessionActive = 1;
}

void Supplicant::setChallenge(char* chal)
{
	for (int i = 0; i < strlen(chal); ++i)
		challenge[i] = chal[i];
}

void Supplicant::setLogin(char* log)
{
	for (int i = 0; i < strlen(log); ++i)
		login[i] = log[i];
}

void Supplicant::setPassword(char* pas)
{
	for (int i = 0; i < strlen(pas); ++i)
		password[i] = pas[i];
}

int Supplicant::eapolStart()
{
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

	return 0;
}


int Supplicant::eapolLogoff()
{
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

	return 0;
}



int Supplicant::eapResponseIdentify()
{
	u_char packet_buffer[100];
	ETHERNET_HEADER* eth;
	EAP_HEADER* eap;

	eth = (ETHERNET_HEADER*)packet_buffer;
	memcpy(eth->destination, destinationMac, 6);
	memcpy(eth->source, sourceMac, 6);
	eth->type = htons(0x888E);
	eth->protocol_version = 2;
	eth->packet_type = 0;
	eth->packet_body_length = sizeof(EAP_HEADER) + strlen(login);

	eap = (EAP_HEADER*)(packet_buffer + sizeof(ETHERNET_HEADER));
	eap->code = 2;
	eap->identifier = connectionIdentifier;
	eap->length = htons(0x0006);

	cout << strlen(login) << endl;

	char* data;
	data = (char *)(packet_buffer + sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER));
	*data = 0x1;
	data = (char*)(packet_buffer + sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER) + 1);
	strcpy_s(data, strlen(login) + 1, login);

	cout << sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER) + strlen(login) + 1 << endl; //TODO sprawdziæ czy œmiga

	if (pcap_sendpacket(fp, packet_buffer, sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER) + strlen(login) + 1) != 0)
	{
		fprintf(stderr, "\nError sending the EAPOL-Start packet: \n", pcap_geterr(fp));
		return 1;
	}

	return 0;
}

int Supplicant::eapResponseChallenge()
{
	string pas(password);
	string cha(challenge);
	string res = pas + cha;

	const char *res2 = res.c_str();

	MD5 md5;
	md5.add(res2, res.length());
	cout << md5.getHash() << " skrot" << endl;

	const char *res3 = (md5.getHash()).c_str();

	ETHERNET_HEADER* eth;
	EAP_HEADER* eap;
	u_char packet_buffer[100];

	eth = (ETHERNET_HEADER*)packet_buffer;
	memcpy(eth->destination, destinationMac, 6);
	memcpy(eth->source, sourceMac, 6);
	eth->type = htons(0x888E);
	eth->protocol_version = 2;
	eth->packet_type = 0;
	eth->packet_body_length = sizeof(EAP_HEADER) + 17; // 1 - type , 16 - skrot md5

	eap = (EAP_HEADER*)(packet_buffer + sizeof(ETHERNET_HEADER));
	eap->code = 2;
	eap->identifier = connectionIdentifier;
	eap->length = htons(0x0011);

	char *data;
	data = (char *)(packet_buffer + sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER));
	*data = 0x4;
	data = (char *)(packet_buffer + sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER) + 1);
	for (int i = 0; i < 16; ++i)
		*(data + i) = md5.getHash()[i];


	if (pcap_sendpacket(fp, packet_buffer, sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER) + 17) != 0)
	{
		fprintf(stderr, "\nError sending the EAP-MD5-Challenge Response packet: \n", pcap_geterr(fp));
		return 1;
	}

	return 0;
}

int Supplicant::listen()
{
	char packet_filter[] = "ether proto 0x888E";	//magiczny filtr dopuszczajacy tylko eapol
	struct bpf_program fcode;

	cout << "!!";

	u_int netmask;
	if (d->addresses != NULL)
		/* Retrieve the mask of the first address of the interface */
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* If the interface is without addresses we suppose to be in a C class network */
	{
		netmask = 0xffffff; 
	}


	if (pcap_compile(fp, &fcode, packet_filter, 1, netmask) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");


		return -1;
	}

	//set the filter
	if (pcap_setfilter(fp, &fcode) < 0)
	{
		fprintf(stderr, "\nError setting the filter.\n");

		return -1;
	}


	printf("\nlistening on %s...\n", d->description);
	
	//pcap_loop(fp, 0, Listener::packetListener, NULL);
	return 0;
}

void Supplicant::listenNext()
{
	bool breaker = true;
	char packet_filter[] = "ether proto 0x888E";	//magiczny filtr dopuszczajacy tylko eapol
	struct bpf_program fcode;

	cout << "!!";

	u_int netmask;
	if (d->addresses != NULL)
		/* Retrieve the mask of the first address of the interface */
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* If the interface is without addresses we suppose to be in a C class network */
	{
		netmask = 0xffffff;
	}


	if (pcap_compile(fp, &fcode, packet_filter, 1, netmask) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");


		return;
	}

	//set the filter
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
		struct tm ltime;
		char timestr[16];
		time_t local_tv_sec;
		u_char temp[100];

		if (res == 0)
			continue;

		++packet_counter;	

		int len = header->len;
		cout << "Packet Received: (nr:" <<packet_counter <<")"<< len << endl;


		for (int i = 0; i < len; ++i)
		{
			temp[i] = *(pkt_data + i);
		}
		cout << endl;

		ETHERNET_HEADER* eth;
		EAP_HEADER* eap;
		char* data;
		//data = (char*)(temp + sizeof(ETHERNET_HEADER)+sizeof(EAP_HEADER));
		eth = (ETHERNET_HEADER*)temp;
		//cout << eth -> packet_type<< endl;
		//printf("%.2X", eth->packet_type);


		switch (eth->packet_type){
		case 0x00:
			cout << "packet" << endl;
			eap = (EAP_HEADER*)(temp + sizeof(ETHERNET_HEADER));

			switch (eap->code){
			case 0x01:
				cout << "req" << endl;
				data = (char*)(temp + sizeof(ETHERNET_HEADER)+sizeof(EAP_HEADER));
				cout << "type:" << endl;

				if (*data == (char)0x1)
				{
					cout << "Identify" << endl;
					eapResponseIdentify();
					//Zawsze jak chcesz wyslac pakiet to breaker = 0 ; to wylaczy listener i dzieki temu nie odczytamy wlasnego pakietu.
					breaker = 0;
				}

				//TODO inne pakiety ? Notification(0x2)? Nak(0x3) ?

				if (*data == (char)0x4)
				{
					cout << "MD5_Challenge" << endl;
					//data += 2; ciekawe czy tak moge.
					data = (char*)(temp + sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER) + 2);
					// [type(1)][value_size(1)!!][value][name] // TODO sprawdzic(3.4 RFC);
					char temp2[100];
					for (int i = 0; i < 16; ++i)
					{
						temp2[i] = (char)*(data + i);
					}
					setChallenge(temp2);

					eapResponseIdentify(); // TODO naprawiæ ramkê MD5 challenge ( patrz 3.4 RFC)
					//wylacz listener
					breaker = 0;
				}


				printf("%.2X", *data);
				break;
			case 0x02:
				//illegal packet

				/*
				cout << "respo" << endl;
				data = (char*)(temp + sizeof(ETHERNET_HEADER)+sizeof(EAP_HEADER));
				cout << "type:" << endl;
				if (*data == (char)0x1)
					cout << "JES" << endl;
				printf("%.2X", *data);
				eapResponseIdentify();
				//eapolLogoff();
				breaker = 0;
				cout << endl;
				*/
				break;

			case 0x03:
				cout << "Success: Client authentication accepted. Access to network granted. " << endl;
				//Start console
				//Start packet transmission
				//when done:
				// sessionActive = 0;
				breaker = 0;
				break;
			case 0x04:
				cout << "fail" << endl;
				break;
			default:
				cout << "code: def" << endl;
			}

			break;
		case 0x01:
			cout << "start" << endl;
			break;
		case 0x02:
			cout << "logoff" << endl;
			break;
		default:
			cout << "type: def " << endl;
		}
	}
}