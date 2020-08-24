#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>

int main(int argc, char *argv[]) {
	struct input_event event;
	if(argc < 1)
		return 1;
	const char *file_name = argv[1];
	int fd = open(file_name, O_RDWR);
	if(fd < 0) {
		fprintf(stderr, "open %s error: %m\n", file_name);
		return 1;
	}
	while(1) {
		memset((void*)&event, 0, sizeof(event));
		int ret = read(fd, &event, sizeof(event));
		if(ret == sizeof(event)) {
			if(event.type == 1) {
				fprintf(stdout, "code: %d   value:%d\n", event.code, event.value);
			}
		}
		else {
			break;
		}
	}
	close(fd);
	return 0;
}
