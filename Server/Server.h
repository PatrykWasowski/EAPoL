#pragma once
#pragma comment(lib,"ws2_32.lib")

#include <memory.h>
#include <pcap.h>
#include "../headers.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include "../md5.h"

using namespace std;

extern pcap_t *fp;

extern u_char SUPPLICANT_MAC[6];
extern u_char SERVER_MAC[6];
extern char CHALLENGE_STRING[];
extern u_char PASSWORD[];
extern u_char CONNECTION_IDENTIFIER;

extern bool ACCES_GRANTED;


void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
int init();
int EAP_request_identify();
int EAP_request_MD5();
int EAP_succes();
int EAP_failure();