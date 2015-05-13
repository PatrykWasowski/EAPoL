#pragma once
#include <pcap.h>
#include "headers.h"
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <string>
#pragma comment(lib,"ws2_32.lib")




int CONNECTION_IDENTIFIER = 123;
u_char SUPPLICANT_MAC[6];
u_char SERVER_MAC[6];

pcap_t *fp;


int EAP_request_identify();
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
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

	EAP_request_identify();

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
	eap->identifier = CONNECTION_IDENTIFIER;
	u_char type = 1;//identify
	eap->length = htons(sizeof(eap_header)+1); //1 bajt na type

	packet_buffer[sizeof(EAP_HEADER) + sizeof(ETHERNET_HEADER)] = type; //offset to naglowek ethernet + naglowek eap

	if (pcap_sendpacket(fp, packet_buffer, sizeof(ETHERNET_HEADER) + sizeof(EAP_HEADER)) != 0)
	{
		fprintf(stderr, "\nError sending the EAPOL-Start packet: \n", pcap_geterr(fp));
		return 1;
	}
	printf("wyslano eap_request_identify\n");
	return 0;
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
	default:
		printf("default");
	}

}