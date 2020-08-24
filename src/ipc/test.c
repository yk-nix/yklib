#include <stdio.h>
#include <unistd.h>
#include <string.h>

void main(int argc, char *argv[]) {
	while(1) {
		printf("hello\n");
		usleep(1000000);
	}
}
