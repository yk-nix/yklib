#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/un.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>

void ifconf_print_list() 
{
	struct if_nameindex *ifs = if_nameindex();
	struct if_nameindex *p = ifs;
	for(; p && p->if_index; p++) {
		printf("%d   %s\n", p->if_index, p->if_name);
	}
	if_freenameindex(ifs);
}
char *hwaddr2str(struct sockaddr *addr, char *buf, size_t len) 
{
			(unsigned char)addr->sa_data[0],
			(unsigned char)addr->sa_data[1],
			(unsigned char)addr->sa_data[2],
			(unsigned char)addr->sa_data[3],
			(unsigned char)addr->sa_data[4],
			(unsigned char)addr->sa_data[5]);
	return buf;
}
char *sockaddr2str(struct sockaddr *addr, char *buf, size_t len) 
{
	switch(addr->sa_family) {
	case AF_INET:
	case AF_INET6: {
		struct sockaddr_in *sin = (struct sockaddr_in *)addr;
		char ip[64];
		memset(ip, 0, sizeof(ip));
		snprintf(buf, len, "%s", inet_ntop(addr->sa_family, &(sin->sin_addr), ip, sizeof(ip)));
		break;
	}
	case AF_UNIX: {
		struct sockaddr_un *un = (struct sockaddr_un *)addr;
		snprintf(buf, len, "%s", un->sun_path);
		break;
	}
	case AF_PACKET: {
		struct sockaddr_ll *ll = (struct sockaddr_ll *)addr;
		snprintf(buf, len, "%02x:%02x:%02x:%02x:%02x:%02x", 
				   ll->sll_addr[0], 
				   ll->sll_addr[1], 
				   ll->sll_addr[2], 
				   ll->sll_addr[3], 
				   ll->sll_addr[4], 
				   ll->sll_addr[5]);
		break;
	}
	default:
		return NULL;
	}
	return buf;
}

#define MAX_IFS    64
void ifconf_dump_all() 
{
	struct if_nameindex *if_names = if_nameindex();
	struct if_nameindex *p = if_names;
	struct ifreq ifreq;
	struct ifreq ifs[MAX_IFS];
	struct ifconf ifconf = {
		.ifc_len = sizeof(ifs),
		.ifc_req = ifs,	
	};
	char buf[256];
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(ioctl(sock, SIOCGIFCONF, &ifconf)) {
		printf("%d   %s: error: %m\n", p->if_index, p->if_name);
		return;
	}
	int i = 0;
	struct sockaddr *addr;
	if(ioctl(sock, SIOCGIFCOUNT, &ifreq)) {
	printf("SIOCGIFCOUNT: %m\n");
	for(; i * sizeof(ifreq) < ifconf.ifc_len; i++) {
		memcpy(&ifreq, &ifs[i], sizeof(ifreq));
		printf("%d   %s\n", i, ifreq.ifr_name);
		printf("\taddress: %s\n", sockaddr2str(&ifreq.ifr_addr, buf, sizeof(buf))); 
		
		if(!ioctl(sock, SIOCGIFNETMASK, &ifreq)) {
			memset(buf, 0, sizeof(buf));
			printf("\tnetmask: %s\n", sockaddr2str(&ifreq.ifr_netmask, buf, sizeof(buf)));
		}
		if(!ioctl(sock, SIOCGIFHWADDR, &ifreq)) {
			memset(buf, 0, sizeof(buf));
			printf("\tMAC: %s\n", hwaddr2str(&ifreq.ifr_hwaddr, buf, sizeof(buf)));
		}
		if(!ioctl(sock, SIOCGIFBRDADDR, &ifreq)) {
			memset(buf, 0, sizeof(buf));
			printf("\tbroadcast: %s\n", hwaddr2str(&ifreq.ifr_broadaddr, buf, sizeof(buf)));
		}
		if(!ioctl(sock, SIOCGIFMETRIC, &ifreq)) {
			printf("\tmetric: %d\n", ifreq.ifr_metric);
		}
		if(!ioctl(sock, SIOCGIFMTU, &ifreq)) {
			printf("\tmtu: %d\n", ifreq.ifr_mtu);
		}
		if(!ioctl(sock, SIOCGIFTXQLEN, &ifreq)) {
			printf("\tqlen: %d\n", ifreq.ifr_qlen);
		}
	}

}
int main(int argc, char *argv[]) 
{
	ifconf_dump_all();
}
