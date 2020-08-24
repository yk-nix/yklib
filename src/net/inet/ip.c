
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

/*
 * send ip packet
 */

int yk_ip_send(const char *msg, size_t len, const char *dstip) {	
	int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if(sock < 0)
		return -1;
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = inet_addr(dstip),
	};
	int bytes = sendto(sock, msg, len, 0, (struct sockaddr *)&addr, sizeof(addr));
	close(sock);
	return bytes;
}

