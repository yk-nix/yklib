#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>

#define err(msg) \
do { \
	perror(msg); \
	exit(1); \
} while(0) 

int main(int argc, char *argv[])
{
	int master = open("/dev/pts/ptmx", O_RDWR);
	if(master < 0)
		err("open");
	printf("%s\n", ptsname(master));
	while(1) {
		usleep(500000);
	}
}
