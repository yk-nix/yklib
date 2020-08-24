#include <stdio.h>
#include <signal.h>
#include <yk_lib.h>

#if defined(__sighandler_t)
#define sighandler_t  __sighandler_t
#else
typedef void (*sighandler_t)(int);
#endif

yk_dict_t yk_signal_names[] = {
	{SIGHUP,  "sighup",  NULL, ""},
	{SIGINT,  "sigint",  NULL, ""},
	{SIGQUIT, "sigquit", NULL, ""},
	{SIGILL,  "sigill",  NULL, ""},
	{SIGTRAP, "sigtrap", NULL, ""},
	{SIGABRT, "sigabrt", NULL, ""},
	{SIGIOT,  "sigiot",  NULL, ""},
	{SIGBUS,  "sigbus",  NULL, ""},
	{SIGFPE,  "sigfpe",  NULL, ""},
	{SIGKILL, "sigkill", NULL, ""},
	{SIGUSR1, "sigusr1", NULL, ""},
	{SIGSEGV, "sigsegv", NULL, ""},
	{SIGUSR2, "sigusr2", NULL, ""},
	{SIGPIPE, "sigpipe", NULL, ""},
	{SIGALRM, "sigalrm", NULL, ""},
	{SIGTERM, "sigterm", NULL, ""},
	{SIGSTKFLT, "sigstkflt", NULL, ""},
	{SIGCHLD, "sigchld", NULL, ""},
	{SIGCONT, "sigcont", NULL, ""},
	{SIGSTOP, "sigstop", NULL, ""},
	{SIGTSTP, "sigtstp", NULL, ""},
	{SIGTTIN, "sigttin", NULL, ""},
	{SIGTTOU, "sigttou", NULL, ""},
	{SIGURG,  "sigurg",  NULL, ""},
	{SIGXCPU, "sigxcpu", NULL, ""},
	{SIGXFSZ, "sigxfsz", NULL, ""},
	{SIGVTALRM, "sigvtalrm", NULL, ""},
	{SIGPROF, "sigprof", NULL, ""},
	{SIGWINCH,"sigwinch",NULL, ""},
	{SIGIO,   "sigio",   NULL, ""},
	{SIGPWR,  "sigpwr",  NULL, ""},
	{SIGSYS,  "sigsys",  NULL, ""},
	{YK_KEY_INVALID, "", NULL, ""}
};

void yk_signal_print_list(void) {
	int i = 1;
	char *name;
	for(; i < SIGRTMIN; i++) {
		name = yk_dict_id2name(yk_signal_names, i);
		if(name)
			fprintf(stdout, "%.2d)%-10s", i, yk_dict_id2name(yk_signal_names, i));
		if(i % 4 == 0)
			fprintf(stdout, "\n");
	}
	if(i % 4 != 0)
		fprintf(stdout, "\n");
}

void yk_signal_dump_sigset(sigset_t *s) {
	int i = 1, count = 0;
	char *name;
	for(; i < SIGRTMIN; i++) {
		if(sigismember(s, i)) {
			fprintf(stdout, "%.2d)%-10s", i, yk_dict_id2name(yk_signal_names, i));
			count++;
		}
		if(count && count % 4 == 0)
			fprintf(stdout, "\n");
	}
	if(count % 4 != 0)
		fprintf(stdout, "\n");
}

void yk_signal_register_sigset(sigset_t *s, sighandler_t func) {
	int i = 0;
	for(; i < YK_ARRAY_SIZE(yk_signal_names); i++) {
		if(sigismember(s, yk_signal_names[i].key))
			signal(yk_signal_names[i].key, func);
	}
}

