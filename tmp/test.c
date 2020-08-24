#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sched.h>
#include <pthread.h>
#include <unistd.h>

#include "yk_lib.h"
#include "yk_signal.h"
#include "yk_sched.h"


void sig_handler(int signo, siginfo_t *siginfo, void *ctx) {
	printf("%x: signal(%d) received\n", pthread_self(), signo);
}

void *thread_routine1(void *arg) {
	printf("%x: started\n", pthread_self());
	int i = 0;
	printf("last alarm: %d\n", alarm(10));
	while(1) {
		i++;	
	}
}

void *thread_routine2(void *arg) {
	printf("%x: started\n", pthread_self());
	int i = 0;
	printf("last alarm: %d\n", alarm(10));
	while(1) {
		i++;	
	}
}



void main(int argc, char *argv[]) {
	DEFINE_SIGSET(s, SIGINT);
	yk_signal_dump_sigset(&s);
}


