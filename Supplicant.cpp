#include "Supplicant.h"

using namespace std;

void Supplicant::init()
{

	pcap_if_t *alldevs;
	pcap_if_t *d;
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
		return ;
	}

	printf("Enter the interface number (1-%d):", i);
	scanf_s("%d", &inum);

	if (inum < 1 || inum > i)
	{
		printf("\nInterface number out of range.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return ;
	}

	/* Jump to the selected adapter */
	for (d = alldevs, i = 0; i< inum - 1; d = d->next, i++);


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

	login = "Maciek";
	password = "pass";
	challenge = "chal";

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
	data = (char*)(packet_buffer + sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER));
	strcpy_s(data, strlen(login)+1, login);

	cout << sizeof(ETHERNET_HEADER)+sizeof(EAP_HEADER)+strlen(login) << endl; 

	if (pcap_sendpacket(fp, packet_buffer, sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER) + strlen(login)) != 0)
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
	data = (char *)(packet_buffer + sizeof(ETHERNET_HEADER)+sizeof(EAP_HEADER));
	*data = 0x4;
	data = (char *)(packet_buffer + sizeof(ETHERNET_HEADER)+sizeof(EAP_HEADER)+1);
	for (int i = 0; i < 17; ++i)
		*(data + i) = md5.getHash()[i];
	
	
	if (pcap_sendpacket(fp, packet_buffer, sizeof(ETHERNET_HEADER)+sizeof(EAP_HEADER)+17) != 0)
	{
		fprintf(stderr, "\nError sending the EAP-MD5-Challenge Response packet: \n", pcap_geterr(fp));
		return 1;
	}

	return 0;
}