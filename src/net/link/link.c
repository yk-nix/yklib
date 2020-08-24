#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static struct ifreq *yk_if_req(struct ifreq *req, int cmd) {
	int sock;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0)
		return NULL;
	if(ioctl(sock, cmd, req) < 0) {
		close(sock);
		return NULL;
	}
	close(sock);
	return req;
}

char *yk_if_getaddr(const char *ifname) {
	struct ifreq ifreq;
	struct sockaddr_in *addr;
	
	memset((void *)&ifreq, 0, sizeof(ifreq));
	sprintf(ifreq.ifr_name, "%s", ifname);
	if(!yk_if_req(&ifreq, SIOCGIFADDR))
		return NULL;
	addr = (struct sockaddr_in *)&ifreq.ifr_addr;
	if(addr->sin_family != AF_INET)
		return NULL;
	return (inet_ntoa(addr->sin_addr));
}




