#include "Supplicant.h"

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


	if ((fp = pcap_open(d->name,        // name of the device
		100,						// portion of the packet to capture (only the first 100 bytes)
		PCAP_OPENFLAG_PROMISCUOUS,  // promiscuous mode
		1000,						// read timeout
		NULL,						// authentication on the remote machine
		errbuf						// error buffer
		)) == NULL)
			;


			
	u_char packet[50];
	packet[0] = (u_char)0;
	packet[1] = (u_char)80;
	packet[2] = (u_char)86;
	packet[3] = (u_char)192;
	packet[4] = (u_char)0;
	packet[5] = (u_char)1;
	for (int i = 6; i < 50; ++i)
		packet[i] = (u_char)1;

	if (pcap_sendpacket(fp, packet, 50) != 0)
	{
		fprintf(stderr, "\nError sending the packet: \n", pcap_geterr(fp));
		return;
	}

}