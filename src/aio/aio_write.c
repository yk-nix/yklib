#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <aio.h>
#include <time.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#include "../com/yk_tool.h"

static struct aiocb aiocb = {
	.aio_offset = 0,
	.aio_reqprio = 0,
};

static int port = 1234;
static char *hostip = "127.0.0.1";

void init(int argc, char *argv[])
{
	char opt;
	while((opt = getopt(argc, argv, "-p:")) != -1){
		switch(opt){
		case 'p':
			if(atoi(optarg) > 0)
				port = atoi(optarg);
			break;
		case 1:
			if(optarg)
				hostip = optarg;
			break;
		default:
			break;
		}
	}
	
}

void main(int argc, char *argv[])
{
	char buf[1024];

	init(argc, argv);

	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0){
		printf("error: socket: %s\n", strerror(errno));
		return;
	}

	struct sockaddr_in remoteaddr = {
		.sin_family = AF_INET,
		.sin_port   = htons(port),
		.sin_addr.s_addr = inet_addr(hostip),
	};
	
	if(connect(sock, (struct sockaddr *)&remoteaddr, sizeof(struct sockaddr_in))){
		printf("error: connect: %s\n", strerror(errno));
		goto Done;	
	}

	memset(buf, 0, sizeof(buf));
	aiocb.aio_buf = buf;
	aiocb.aio_fildes = sock;
	while(fgets(buf, sizeof(buf), stdin) != NULL){
		if(!strncasecmp(buf, "quit", 4) || !strncasecmp(buf, "exit", 4))
			break;
		aiocb.aio_nbytes = strlen(buf);
		if(aio_write(&aiocb)){
			printf("error: aio_write: %s\n", strerror(errno));
			break;
		}
		while(aio_error(&aiocb) == EINPROGRESS){
			usleep(200000);
		}
		printf("\n");
	}
	
Done:
	close(sock);
}
