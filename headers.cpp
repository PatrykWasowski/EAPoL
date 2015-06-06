#include <pcap.h>

typedef struct ethernet_header {
	u_char destination[6];
	u_char source[6];
	u_short type;
	u_char protocol_version;
	u_char packet_type;
	u_short packet_body_length;
} ETHERNET_HEADER;

typedef struct eap_header{
	u_char code;
	u_char identifier;
	u_short length;
} EAP_HEADER;

typedef struct eapol_header {
	u_char descriptor;
	u_short key_length;
	long long int relay_counter;
	u_char key_iv[16];
	u_char key_index;
	u_char key_signature[16];
} EAPOL_HEADER;