#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <time.h>
#include <aio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>


static int port = 1234;

void init(int argc, char *argv[])
{
	char opt;
	while((opt = getopt(argc, argv, "p:")) != -1){
		switch(opt){
		case 'p':
			if(atoi(optarg) > 0)
				port = atoi(optarg);
			break;
		default:
			break;
		}
	}
}

void main(int argc, char *argv[])
{
	init(argc, argv);
	
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0){
		printf("error: socket: %s\n", strerror(errno));
		return;
	}

	struct sockaddr_in localaddr = {
		.sin_family = AF_INET,
		.sin_port = htons(port),
		.sin_addr.s_addr = htonl(INADDR_ANY),
	};

	if(bind(sock, (struct sockaddr *)&localaddr, sizeof(struct sockaddr_in))){
		printf("error: bind: %s\n", strerror(errno));
		goto Done;
	}
	
	char buf[1024];
	struct aiocb aiocb = {
		.aio_fildes = sock,
		.aio_buf = buf,
		.aio_nbytes = sizeof(buf),
		.aio_offset = 0,
		.aio_reqprio = 0,
	};
	
	while(1){
		memset(buf, 0, sizeof(buf));
		if(aio_read(&aiocb)){
			printf("error: aio_read: %s\n", strerror(errno));
			break;
		}
		while(aio_error(&aiocb) == EINPROGRESS){
			usleep(100000);
		}
		printf("%s", buf);
	}
	
Done:
	close(sock);
}
