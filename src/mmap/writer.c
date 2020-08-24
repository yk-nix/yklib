#include <sys/mman.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
	off_t off = 0;
	char *cache;
	int len = 512;
	int fd = open("cache", O_RDWR);
	if(fd < 0) {
		printf("open error: %s\n", strerror(errno));
		return -1;
	}
	cache = mmap(NULL, len, PROT_WRITE, MAP_SHARED, fd, 0);
	if(cache == MAP_FAILED) {
		printf("mmap error: %s\n", strerror(errno));
		close(fd);
		return -1;
	}
	close(fd);
	while(1) {
		scanf("%s", cache);
		if(strcasecmp(cache, "exit") == 0)
			break;
	}
	return 0;
}
