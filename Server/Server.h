#pragma once
#pragma comment(lib,"ws2_32.lib")

#include <memory.h>
#include <pcap.h>
#include "../headers.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include "../md5.h"



u_char SUPPLICANT_MAC[6];
u_char SERVER_MAC[6];

char CHALLENGE_STRING[]="CHALLENGE";
u_char PASSWORD[] = "PASSWORD";
u_char CONNECTION_IDENTIFIER=123;

pcap_t *fp;

bool ACCES_GRANTED = false;

void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

int init();

int EAP_request_identify();
int EAP_request_MD5();
int EAP_succes();
int EAP_failure();

int init(){
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int inum;
	int i = 0;

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
		return -1;
	}

	printf("Enter the interface number (1-%d):", i);
	scanf_s("%d", &inum);

	if (inum < 1 || inum > i)
	{
		printf("\nInterface number out of range.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* Jump to the selected adapter */
	for (d = alldevs, i = 0; i< inum - 1; d = d->next, i++);

	/* Open the device */
	if ((fp = pcap_open(d->name,          // name of the device
		65536,            // portion of the packet to capture
		// 65536 guarantees that the whole packet will be captured on all the link layers
		PCAP_OPENFLAG_PROMISCUOUS,    // promiscuous mode
		1000,             // read timeout
		NULL,             // authentication on the remote machine
		errbuf            // error buffer
		)) == NULL)
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}


	u_int netmask;
	if (d->addresses != NULL)
		/* Retrieve the mask of the first address of the interface */
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* If the interface is without addresses we suppose to be in a C class network */
		netmask = 0xffffff;
	struct bpf_program fcode;

	char packet_filter[] = "ether proto 0x888E";	//magiczny filtr dopuszczajacy tylko eapol


	if (pcap_compile(fp, &fcode, packet_filter, 1, netmask) <0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	//set the filter
	if (pcap_setfilter(fp, &fcode)<0)
	{
		fprintf(stderr, "\nError setting the filter.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}


	printf("\nlistening on %s...\n", d->description);
	/* At this point, we don't need any more the device list. Free it */
	pcap_freealldevs(alldevs);
	/* start the capture */
	pcap_loop(fp, 0, packet_handler, NULL);

	return 0;

}


int EAP_request_identify()
{
	u_char packet_buffer[100];
	ETHERNET_HEADER* eth;
	EAP_HEADER* eap;

	eth = (ETHERNET_HEADER*)packet_buffer;
	memcpy(eth->destination, SUPPLICANT_MAC, 6);
	memcpy(eth->source, SERVER_MAC, 6);
	eth->type = htons(0x888E);
	eth->protocol_version = 2;
	eth->packet_type = 0; //eapol-packet
	eth->packet_body_length = sizeof(EAP_HEADER) + 1; //1 bajt na type


	eap = (EAP_HEADER*)(packet_buffer + sizeof(ETHERNET_HEADER));
	eap->code = 1;	//request
	eap->identifier = CONNECTION_IDENTIFIER; // TODO to ma by� zmienne(losowe?)
	u_char type = 1;//identify
	eap->length = htons(sizeof(eap_header) + 1); //1 bajt na type

	char *data = (char *)(packet_buffer + sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER));
	*data = type;
	//za typem mozna dopisac jeszcze wiadomosc - ale wtedy nalezy poprawic dlugosc pakietow


	if (pcap_sendpacket(fp, packet_buffer, sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER) + 1) != 0)
	{
		fprintf(stderr, "\nError sending the EAPOL-request_identify: \n", pcap_geterr(fp));
		return 1;
	}
	printf("wyslano eap_request_identify\n");
	return 0;
}

int EAP_request_MD5()
{


	ETHERNET_HEADER* eth;
	EAP_HEADER* eap;
	u_char packet_buffer[100];

	eth = (ETHERNET_HEADER*)packet_buffer;
	memcpy(eth->destination, SUPPLICANT_MAC, 6);
	memcpy(eth->source, SERVER_MAC, 6);
	eth->type = htons(0x888E);
	eth->protocol_version = 2;
	eth->packet_type = 0;	//eap-packet
	eth->packet_body_length = sizeof(EAP_HEADER) + 1 + strlen(CHALLENGE_STRING); // 1 - type

	eap = (EAP_HEADER*)(packet_buffer + sizeof(ETHERNET_HEADER));
	eap->code = 1; //request
	eap->identifier = CONNECTION_IDENTIFIER; //TODO To ma by� zmienne (losowe?)
	eap->length = htons((sizeof(EAP_HEADER) + 2 + strlen(CHALLENGE_STRING)));	//1 bajt na type, 1 bajt na dlugosc challenge_Stringa
	u_char type = 4;//MD5-challenge

	char *type_adress = (char *)(packet_buffer + sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER));
	*type_adress = type;

	char *data = type_adress + 1;
	u_char value_size = strlen(CHALLENGE_STRING);
	*data = value_size;
	data++;
	for (int i = 0; i < strlen(CHALLENGE_STRING); ++i)
		data[i] = CHALLENGE_STRING[i];

	if (pcap_sendpacket(fp, packet_buffer, sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER) + 2 + strlen(CHALLENGE_STRING)) != 0)
	{
		fprintf(stderr, "\nError sending the EAP-MD5-Challenge Request packet: \n", pcap_geterr(fp));
		return 1;
	}
	printf("wyslano eap_request_md5\n");
	return 0;
}

int EAP_succes(){
	u_char packet_buffer[100];
	ETHERNET_HEADER* eth;
	EAP_HEADER* eap;

	eth = (ETHERNET_HEADER*)packet_buffer;
	memcpy(eth->destination, SUPPLICANT_MAC, 6);
	memcpy(eth->source, SERVER_MAC, 6);
	eth->type = htons(0x888E);
	eth->protocol_version = 2;
	eth->packet_type = 0; //eapol-packet
	eth->packet_body_length = sizeof(EAP_HEADER);

	eap = (EAP_HEADER*)(packet_buffer + sizeof(ETHERNET_HEADER));
	eap->code = 3;	//succes
	eap->identifier = CONNECTION_IDENTIFIER;
	eap->length = htons(sizeof(eap_header));


	if (pcap_sendpacket(fp, packet_buffer, sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER)) != 0)
	{
		fprintf(stderr, "\nError sending the EAP_succes: \n", pcap_geterr(fp));
		return 1;
	}
	printf("wyslano eap_succes\n");
	return 0;
}

int EAP_failure(){
	u_char packet_buffer[100];
	ETHERNET_HEADER* eth;
	EAP_HEADER* eap;

	eth = (ETHERNET_HEADER*)packet_buffer;
	memcpy(eth->destination, SUPPLICANT_MAC, 6);
	memcpy(eth->source, SERVER_MAC, 6);
	eth->type = htons(0x888E);
	eth->protocol_version = 2;
	eth->packet_type = 0; //eapol-packet
	eth->packet_body_length = sizeof(EAP_HEADER);

	eap = (EAP_HEADER*)(packet_buffer + sizeof(ETHERNET_HEADER));
	eap->code = 4;	//failure
	eap->identifier = CONNECTION_IDENTIFIER;
	eap->length = htons(sizeof(eap_header));


	if (pcap_sendpacket(fp, packet_buffer, sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER)) != 0)
	{
		fprintf(stderr, "\nError sending the eap failure: \n", pcap_geterr(fp));
		return 1;
	}
	printf("wyslano eap_failure\n");
	return 0;
}
bool validate_hashes(u_char *eap_header_adress){
	//WYDLUBANIE HASHA Z PAKIETU
	u_char* value_size = (u_char*)eap_header_adress+sizeof(EAP_HEADER) + 1; //1 bo przesuniete o pole type
	if (*value_size != 16){
		printf("dlugosc value_size nie jest rowna 16. czy do hasha doklejono cos jeszcze? wartosc dlugosci: %d\n", *value_size);
	}
	char hash_from_packet[16];
	printf("odebrany hash:\n");
	char* hash_packet_adress = (char*)value_size + 1;
	for (int i = 0; i < 16; i++){
		hash_from_packet[i] = hash_packet_adress[i];
		printf("%c", hash_from_packet[i]);
	}
	printf("\n");

	//OBLICZENIE HASHA
	int string_len = strlen((char*)&CONNECTION_IDENTIFIER) + strlen((char*)PASSWORD) + strlen((char*)CHALLENGE_STRING) + 1;
	char id_pas_chal[100];
	strcpy(id_pas_chal, (char*)&CONNECTION_IDENTIFIER); /* copy name into the new var */
	strcat(id_pas_chal, (char*)PASSWORD); /* add the extension */
	strcat(id_pas_chal, (char*)CHALLENGE_STRING); /* add the extension */
	printf("ident+secret+challenge: %s\n", id_pas_chal);

	MD5 md5;
	md5.add(id_pas_chal, strlen(id_pas_chal));
	const char* calculated_hash = (md5.getHash()).c_str();
	printf("obliczony hash: %s", calculated_hash);

	//POROWNANIE WARTOSCI WYLICZONEJ Z HASHEM Z PAKIETU
	bool hashes_match = true;
	for (int i = 0; i < 16; i++){
		if (hash_from_packet[i] != calculated_hash[i]){
			hashes_match = false;
		}
	}
	std::cout << "czy hashe sie zgadzaja: " << hashes_match << std::endl;
	return hashes_match;
}
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	struct tm ltime;
	char timestr[16];
	time_t local_tv_sec;

	u_char temp[100];

	for (int i = 0; i < header->len; ++i)
	{
		temp[i] = *(pkt_data + i);
	}

	ETHERNET_HEADER* eth;
	EAP_HEADER* eap;
	char* data;
	eth = (ETHERNET_HEADER*)temp;

	//porownanie czy to nie pakiet od samego siebie - jezeli tak to nie obsluguje go
	bool sources_match = true;
	for (int i = 0; i < 6; i++){
		if(eth->source[i] != SERVER_MAC[i])
			sources_match = false;
	}
	if (sources_match){
		return;
	}


	/* convert the timestamp to readable format */
	local_tv_sec = header->ts.tv_sec;
	localtime_s(&ltime, &local_tv_sec);
	strftime(timestr, sizeof timestr, "%H:%M:%S", &ltime);
	

	printf("%s odebrano pakiet. Typ pakietu(eth):%d dlugosc:%d\n", timestr, eth->packet_type, header->len);
	switch (eth->packet_type){
	case 1:	//eap-start
		for (int i = 0; i < 6; i++){
			SUPPLICANT_MAC[i] = eth->source[i];
		}
		EAP_request_identify();
		break;
	case 0: //eap-packet
		eap = (EAP_HEADER*)(temp + sizeof(ETHERNET_HEADER));
		printf("\teap->code pakietu: %d\n", eap->code);
		switch (eap->code){
		case 2://response
			u_char* type = (u_char*)eap + sizeof(EAP_HEADER);
			printf("\t\ttype pakietu : %d\n", *type);
			if (*type == 1){	//identity
				EAP_request_MD5();
			}
			else if (*type == 4){	//md5
				bool hashes_match = validate_hashes((u_char*)eap);
				if (hashes_match){
					ACCES_GRANTED = true;
					EAP_succes();
				}
				else{
					EAP_failure();
				}
			}
			break;
		}
		break;
	case 2: //eap-logoff
		ACCES_GRANTED = false;
		printf("wylogowano\n");
		break;
	}

}