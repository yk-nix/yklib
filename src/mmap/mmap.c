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

int main(int argc, char *argv[]) {
	int ret = 1;
	off_t off = 0;
	char *cache;
	int len = 512;
	if(argc < 2) {
		cache = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED|MAP_LOCKED, -1, 0);
		if(cache == MAP_FAILED)
			return -1;
		int pid = fork();
		if(pid == -1) {
			printf("fork error: %s\n", strerror(errno));
		}
		else if(pid > 0) {
			while(1) {
				scanf("%s", cache);
				if(strcasecmp(cache, "exit") == 0)
					break;
			}
			wait(NULL);
		}
		else if(pid == 0) {
			while(1) {
				printf("%s\n", cache);
				if(strcasecmp(cache, "exit") == 0)
					break;
				usleep(1000000);
			}
		}
		if(munmap(cache, len)) {
			printf("munmap error: %s\n", strerror(errno));
			return -1;
		}
		return 0;
	}
	int fd = open(argv[1], O_RDWR|O_CREAT, 0644);
	if(fd < 0) {
		printf("open file [%s] error: %s\n", argv[0], strerror(errno));
		goto err0;
	}
	if(argc == 3) {
		off = strtol(argv[2], NULL, 0);
		off = off & ~(sysconf(_SC_PAGESIZE) - 1);
	}
	cache = (char *)mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, off);
	if(cache == MAP_FAILED) {
		printf("mmap error: %s\n", strerror(errno));
		close(fd);
		goto err0;
	} close(fd);
	yk_hexdump(cache, 16);
	scanf("%s", cache);
	if(msync(cache, len, MS_SYNC)) {
		printf("msync error: %s\n", strerror(errno));
	}
	scanf("%s", cache);
	yk_hexdump(cache, 16);
	if(munmap(cache, len)) {
		printf("munmap error: %s\n", strerror(errno));
	}
err0:
	return ret;	
}
