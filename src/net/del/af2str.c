#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/socket.h>

static char tmpbuf[16];
static int  getid = 0;

char *af2str(int family){
	memset(tmpbuf, 0, sizeof(tmpbuf));
	switch(family){
	case AF_UNIX:
		sprintf(tmpbuf, "AF_UNIX");
		break;
	case AF_INET:
		sprintf(tmpbuf, "AF_INET");
		break;
	case AF_INET6:
		sprintf(tmpbuf, "AF_INET6");
		break;
	case AF_IPX:
		sprintf(tmpbuf, "AF_IPX");
		break;
	case AF_NETLINK:
		sprintf(tmpbuf, "AF_NETLINK");
		break;
	case AF_X25:
		sprintf(tmpbuf, "AF_X25");
		break;
	case AF_AX25:
		sprintf(tmpbuf, "AF_AX25");
		break;
	case AF_ATMPVC:
		sprintf(tmpbuf, "AF_ATMPVC");
		break;
	case AF_APPLETALK:
		sprintf(tmpbuf, "AF_APPLETALK");
		break;
	case AF_PACKET:
		sprintf(tmpbuf, "AF_PACKET");
		break;
	default:
		sprintf(tmpbuf, "unkown");
		break;
	}
	return tmpbuf;
}

int af2id(const char *family){
	int  i;
	char tmp[16], *p = strchr(family, '_');
	memset(tmp, 0, sizeof(tmp));
	if(p == NULL) return -1;
	else p++;
	for(i=0; i<16 && p[i]!='\0'; i++){
		tmp[i] = toupper(p[i]);
	}

	//printf("%s   ", tmp);
	if(!strcmp(tmp, "UNIX")) return AF_UNIX;
	else if(!strcmp(tmp, "LOCAL")) return AF_LOCAL;
	else if(!strcmp(tmp, "INET")) return AF_INET;
	else if(!strcmp(tmp, "INET6")) return AF_INET6;
	else if(!strcmp(tmp, "IPX")) return AF_IPX;
	else if(!strcmp(tmp, "NETLINK")) return AF_NETLINK;
	else if(!strcmp(tmp, "X25")) return AF_X25;
	else if(!strcmp(tmp, "AX25")) return AF_AX25;
	else if(!strcmp(tmp, "ATMPVC")) return AF_ATMPVC;
	else if(!strcmp(tmp, "APPLETALK")) return AF_APPLETALK;
	else if(!strcmp(tmp, "PACKET")) return AF_PACKET;
	else
		return -1;
}


void init(int argc, char *argv[]){
	char opt;
	while((opt=getopt(argc, argv, "i")) != -1){
		switch(opt){
		case 'i':
			getid = 1;	
			break;
		default:
			break;
		}
	}
}

void main(int argc, char *argv[]){
	init(argc, argv);
	int i = argc;
	while(i-- > 1){
		if(argv[argc - i][0] == '-') continue;
		if(getid)
			printf("%d\n", af2id(argv[argc - i]));
		else
			printf("%s\n", af2str(atoi(argv[argc - i])));	
	}
}
