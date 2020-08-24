#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <pthread.h>

#include "yk_lib.h"
#include "yk_utils.h"
#include "yk_net.h"

struct port_entry {
	
}

int main(int argc, char *argv[]) {
	if(argc < 2)
		return 0;
	int sock = socket(AF_PACKET, SOCK_DGRAM, ETH_P_IP);
	if(sock < 0)
		yk_err("socket");
	struct sockaddr_ll addr = {
		.sll_family   = AF_PACKET,
		.sll_protocol = htons(ETH_P_IP),
		.sll_ifindex  = if_nametoindex(argv[1]),
		.sll_pkttype  = PACKET_HOST,
		.sll_addr     = { 0x40, 0x8d, 0x5c, 0x50, 0x64, 0x03 },
	};
	if(bind(sock, (struct sockaddr *)&addr, sizeof(addr))) {
		close(sock);
		yk_err("bind");
	}

	char cache[2048];
	int bytes = 0;
	struct iphdr *ip = (struct iphdr *)cache;
	while(1) {
		memset(cache, 0, sizeof(cache));
		bytes = recv(sock, cache, sizeof(cache), 0);
		if(bytes > 0 && ip->saddr == inet_addr("192.168.2.233") && 
			ip->protocol == IPPROTO_TCP)
			yk_hexdump(cache, bytes);

	}
}
