#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void exit_hook1(void) {
	printf("%d  bye!\n", getpid());
}

int main(int argc, char *argv[]) {
	atexit(exit_hook1);
	int ret = fork();
	if(ret == 0) {
		return 0;
	}
	else if(ret < 0) {
		printf("error: fork: %m\n");
	}
	return 0;
}
