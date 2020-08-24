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

#define  DEFAULT_PORT   12345

static int  port = DEFAULT_PORT;

void optdump(){
	printf("optind: %d    optopt: %c    optarg: %s     opterr:%d\n",
		optind, optopt, optarg, opterr);
}

int init(int argc, char *argv[]){
	char opt;
	while((opt = getopt(argc, argv, "-p:")) != -1){
		switch(opt){
		case 'p':
			port = atoi(optarg);
			if(!port) port = DEFAULT_PORT;
			break;
		default:
			return -1;
		}
	}
	return 0;
}

void main(int argc, char *argv[]){
	//printf("dst: %s\n", dst);
	//if(msg) printf("msg: %s port: %d\n", msg, port);
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0){
		printf("error: socket: %s\n", strerror(errno));
		return;
	}
	
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port   = htons(port),
		.sin_addr.s_addr = INADDR_ANY
	};
	char   msgbuf[2][1024];
	char   cmsgbuf[CMSG_SPACE(sizeof(struct in_pktinfo))];
	struct in_pktinfo *pktinfo;
	struct sockaddr_in srcaddr;
	struct cmsghdr *cmsghdr;
	struct iovec iomsg[2];
	struct msghdr msghdr = {
		.msg_name        = &srcaddr,
		.msg_namelen     = sizeof(srcaddr),
		.msg_iov         = iomsg,
		.msg_iovlen      = 2,
		.msg_control     = cmsgbuf,
		.msg_controllen  = sizeof(cmsgbuf)
	};
	
	memset((char*)&srcaddr, 0, sizeof(srcaddr));
	memset(msgbuf, 0, sizeof(msgbuf));
	iomsg[0].iov_base = msgbuf[0];
	iomsg[0].iov_len  = 1024;
	iomsg[1].iov_base = msgbuf[1];
	iomsg[1].iov_len  = 1024;
	memset(cmsgbuf, 0, sizeof(cmsgbuf));
	//cmsghdr->cmsg_level = SOL_SOCKET;
	//cmsghdr->cmsg_type  = SCM_RIGHTS;
	int optval = 1;
	setsockopt(sock, IPPROTO_IP, IP_PKTINFO, &optval, sizeof(optval));

	if(bind(sock, (struct sockaddr *)&addr, sizeof(addr))){
		printf("error : bind: %s\n", strerror(errno));
		return;
	}
	
	printf("%#x  %#x --- %#x  %#x  ---  %#x\n", 
		iomsg[0].iov_base, iomsg[1].iov_base, msgbuf[0], msgbuf[1], cmsgbuf);
	printf("namelen: %d   iovlen: %d   controllen: %d\n", msghdr.msg_namelen, msghdr.msg_iovlen, msghdr.msg_controllen);
	while(1){
		int bytes = recvmsg(sock, &msghdr, 0);
		if(bytes < 0){
			printf("error: recvmsg: %s\n", strerror(errno));
		}
		printf("%d bytes received\n", bytes);
		yk_hexdump((const char *)cmsgbuf, sizeof(cmsgbuf));
		yk_hexdump((const char *)&srcaddr, sizeof(srcaddr));
		cmsghdr = CMSG_FIRSTHDR(&msghdr);
		printf("cmsghdr addr: %#x \n", cmsghdr);
		pktinfo = (struct in_pktinfo *)CMSG_DATA(cmsghdr);
		if(cmsghdr){
			printf("cmsg_len: %d       cmsg_level: %d      cmsg_type: %d\n", 
				cmsghdr->cmsg_len, cmsghdr->cmsg_level, cmsghdr->cmsg_type);
			printf("ipi_ifindex: %d   ipi_spec_dst: %s    ipi_addr: %s\n",
				pktinfo->ipi_ifindex, inet_ntoa(pktinfo->ipi_spec_dst), inet_ntoa(pktinfo->ipi_addr));
		}
	}
	close(sock);
}
