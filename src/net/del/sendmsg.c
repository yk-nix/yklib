#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
//#include <linux/in.h>

#define  DEFAULT_PORT   12345

static char dst[16];
static char *msg = NULL;
static int  port = DEFAULT_PORT;

void optdump(){
	printf("optind: %d    optopt: %c    optarg: %s     opterr:%d\n",
		optind, optopt, optarg, opterr);
}

int init(int argc, char *argv[]){
	char opt;
	memset(dst, 0, sizeof(dst));
	sprintf(dst, "localhost");
	while((opt = getopt(argc, argv, "-h:p:")) != -1){
		switch(opt){
		case 'h':
			memset(dst, 0, sizeof(dst));
			sprintf(dst, "%s", optarg);
			break;
		case 'p':
			port = atoi(optarg);
			if(!port) port = DEFAULT_PORT;
			break;
		case 1:
			msg = argv[optind-1];
			//printf("%d  ", opt);
			//optdump();
			break;
		default:
			//printf("%d  ", opt);
			//optdump();
			return -1;
		}
	}
	return 0;
}

void main(int argc, char *argv[]){
	if(init(argc, argv) || msg == NULL) return;
	//printf("dst: %s\n", dst);
	//if(msg) printf("msg: %s port: %d\n", msg, port);
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0){
		printf("error: socket: %s\n", strerror(errno));
		return;
	}

	struct sockaddr_in dstaddr = {
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr.s_addr = inet_addr(dst)
	};
	
	struct in_pktinfo * pktinfo;
	struct iovec iomsg[2];
	char   cmsgbuf[CMSG_SPACE(sizeof(struct in_pktinfo))];
	struct cmsghdr *cmsghdr;
	
	struct msghdr msghdr = {
		.msg_name       = (void*)&dstaddr,
		.msg_namelen    = sizeof(dstaddr),
		.msg_iov        = iomsg,
		.msg_iovlen     = 2,
		.msg_control    = cmsgbuf, 
		.msg_controllen = sizeof(cmsgbuf),
		.msg_flags      = 0,
	};
	iomsg[0].iov_base = msg;
	iomsg[0].iov_len  = strlen(msg);
	iomsg[1].iov_base = "yoka";
	iomsg[1].iov_len  = strlen("yoka");
	cmsghdr = CMSG_FIRSTHDR(&msghdr);
	//cmsghdr->cmsg_type = SCM_RIGHTS;
	//cmsghdr->cmsg_level = SOL_SOCKET;
	cmsghdr->cmsg_type = IP_PKTINFO;
	cmsghdr->cmsg_level = IPPROTO_IP;
	cmsghdr->cmsg_len = CMSG_LEN(sizeof(int));
	pktinfo = (struct in_pktinfo*)CMSG_DATA(cmsghdr);
	memset(pktinfo, 0, sizeof(struct in_pktinfo));
	pktinfo->ipi_ifindex = 1;
	pktinfo->ipi_spec_dst.s_addr = inet_addr(dst);
	pktinfo->ipi_addr.s_addr = inet_addr("192.168.2.241");
	
	int optval = 1;
	setsockopt(sock, IPPROTO_IP, IP_PKTINFO, &optval, sizeof(optval));
	
	//int bytes = sendmsg(sock, &msghdr, 0);
	int bytes = sendto(sock, msg, strlen(msg), 0, (struct sockaddr *)&dstaddr, sizeof(dstaddr));
	if(bytes < 0){
		printf("error: sendmsg: %s\n", strerror(errno));
	}
	
	close(sock);
}
