#include <time.h>
#include <stdio.h> 
#include <unistd.h>
#include <string.h>
#include <errno.h>

static void print_pidinfo(void) {
	printf("pid: %d    ppid: %d    pgid: %d    sid:%d\n", 
		getpid(), getppid(), getpgrp(), getsid(0));
}

void main(int argc, char *argv[]) {
	pid_t p,c;
	p = fork();
	//setsid();
	if(p == -1) {
		fprintf(stderr, "fork error: %s\n", strerror(errno));
		return;
	}
	else if(p == 0) {
		setsid();
		printf("%d\n", tcgetpgrp(0));
		perror("tcgetpgrp");
		printf("child: ");
		print_pidinfo();
		int i =0; 
		for(i=0; i<3; i++) {
			p = fork();
			if(p == 0) {
				setpgid(0, 0);
				usleep(5000000);
				printf("cchild: ");
				print_pidinfo();
				return;
			}
			else if(p == -1) {
				fprintf(stderr, "fork error: %s\n", strerror(errno));
		                return;
			}
		}
		wait(NULL);
	}
	else {
		printf("parent: ");
		print_pidinfo();
	}
	usleep(2000000);
	//int status = 0;
	//wait(&status);
}
