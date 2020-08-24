#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void main(int argc, char *argv[])
{
	if(argc < 1) return;
	int fd = open(argv[1], O_RDONLY);
	if(fd < 0){
		printf("error: %s\n", strerror(errno));
		return;
	}

	char buf[1024];
	memset(buf, 0, sizeof(buf));
	int bytes = read(fd, buf, sizeof(buf));
	if(bytes <= 0){
		printf("error: %s\n", strerror(errno)); 
		goto exit;
	}
	printf("%s\n", buf);
exit:
	close(fd);
	return;
}
