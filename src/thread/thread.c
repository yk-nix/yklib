
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>
#include <signal.h>

#include <yk_utils.h>


void main(int argc, char *argv[]) {
	sigset_t s;
	if(sigpending(&s)) {
		fprintf(stderr, "error: sigpending: %m\n");
		return;
	}
	yk_hexdump((char*)&s, sizeof(s));
	fprintf(stderr, "%d\n", SIGINT);
}
