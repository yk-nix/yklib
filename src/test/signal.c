#include <stdio.h>
#include <time.h>

#include <yk_lib.h>
#include <yk_signal.h>

void dump_mask_sigset(void) {
	sigset_t s;
	sigemptyset(&s);
	if(sigprocmask(0, NULL, &s)) {
		fprintf(stderr, "error: sigprocmask: %m\n");
		return;
	}
	fprintf(stdout, "signal mask set: \n");
	yk_signal_dump_sigset(&s);
}

void dump_pending_sigset(void) {
	sigset_t s;
	sigemptyset(&s);
	if(sigpending(&s)) {
		fprintf(stdout, "error: sigpending: %m\n");
		return; 
	}
	fprintf(stdout, "pending signal set: \n");
	yk_signal_dump_sigset(&s);
}

int main(int argc, char *argv[]) {
	char buf[64];
	sigset_t s;
	sigemptyset(&s);
	sigaddset(&s, SIGINT);
	sigaddset(&s, SIGKILL);
	sigaddset(&s, SIGSTOP);
	sigaddset(&s, SIGQUIT);
	if(sigprocmask(SIG_BLOCK, &s, NULL)) {
		fprintf(stderr, "error: sigprocmask: %m\n");
		return EXIT_FAILED;
	}

	while(1) {
		memset(buf, 0, sizeof(buf));
		if(!fgets(buf, sizeof(buf), stdin))
			break;
		buf[strlen(buf)-1] = '\0';
		if(strcasecmp(buf, "mask") == 0)
			dump_mask_sigset();
		else if(strcasecmp(buf, "pending") == 0)
			dump_pending_sigset();
		
		if(strcasecmp(buf, "exit") == 0)
			break;
	}
	return EXIT_SUCCESS;
}
