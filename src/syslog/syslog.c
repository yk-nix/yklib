#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <syslog.h>

void syslog_print(char *msg, size_t len) {
	struct sockaddr_un addr = {
		.sun_family = AF_UNIX,
		.sun_path   = "/dev/log",
	};
	int sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(sock > 0) {
		sendto(sock, msg, len, 0, (struct sockaddr *)&addr, sizeof(addr));
		close(sock);
	}
}

#ifdef _GNU_SOURCE
void syslog_printf(char *fmt, ...) {
	int bytes;
	char *msg;
	va_list va;
	va_start(va, fmt);
	bytes = vasprintf(&msg, fmt, va);
	if(bytes > 0)
		syslog_print(msg, (size_t)bytes);
	va_end(va);
}
#else
void syslog_printf(char *fmt, ...) { }
#endif

#ifdef _DEBUG_SYSLOG
#include <getopt.h>
#define NAME_ERROR	-1
#define USAGE_FULL	1
#define USAGE_SIMPLE	2
static int facility = LOG_USER;
static int level = LOG_INFO;
static char *message = NULL;
struct _name {
	char * name;
	int value;
};
struct _name facilities[] = {
	{ "LOG_AUTH", LOG_AUTH },
	{ "LOG_AUTHPRIV", LOG_AUTHPRIV },
	{ "LOG_CRON", LOG_CRON },
	{ "LOG_DAEMON", LOG_DAEMON },
	{ "LOG_FTP", LOG_FTP },
	{ "LOG_KERN", LOG_KERN },
	{ "LOG_LOCAL0", LOG_LOCAL0 },
	{ "LOG_LOCAL1", LOG_LOCAL1 },
	{ "LOG_LOCAL2", LOG_LOCAL2 },
	{ "LOG_LOCAL3", LOG_LOCAL3 },
	{ "LOG_LOCAL4", LOG_LOCAL4 },
	{ "LOG_LOCAL5", LOG_LOCAL5 },
	{ "LOG_LOCAL6", LOG_LOCAL6 },
	{ "LOG_LOCAL7", LOG_LOCAL7 },
	{ "LOG_LPR", LOG_LPR },
	{ "LOG_MAIL", LOG_MAIL },
	{ "LOG_NEWS", LOG_NEWS },
	{ "LOG_SYSLOG", LOG_SYSLOG },
	{ "LOG_USER", LOG_USER },
	{ "LOG_UUCP", LOG_UUCP },
	{ NULL, NAME_ERROR },
};
struct _name levels[] = {
	{ "LOG_EMERG", LOG_EMERG },
	{ "LOG_ALERT", LOG_ALERT },
	{ "LOG_CRIT", LOG_CRIT },
	{ "LOG_WARNING", LOG_WARNING }, 
	{ "LOG_INFO", LOG_INFO }, 
	{ "LOG_DEBUG", LOG_DEBUG },
	{ NULL, NAME_ERROR },
};

static int name_to_value(struct _name dict[], char *name) {
	int i = 0;
	while(dict[i].name != NULL) {
		if(strcmp(dict[i].name, name) == 0)
			break;
		i++;
	}	
	return dict[i].value;
}

static char *value_to_name(struct _name dict[], int value) {
	int i = 0;
	while(dict[i].value != NAME_ERROR) {
		if(value == dict[i].value)
			return dict[i].name;
	}
	return NULL;
}

void print_full_usage(char *prog) {
	printf("\n");
	printf("write message \"msg\" to rsyslogd.\n");
	printf("usage: %s [-f facility] [-l level] msg\n", prog);
	printf("   -f facility		to specify what type of program is logging the message.\n");
	printf("   -l level             level of importance of the message.\n");
	printf("\n");
	printf("valid values of facilities:\n");
	printf("  LOG_AUTH, LOG_AUTHPRIV, LOG_CRON, LOG_DAEMON, LOG_FTP, LOG_KERN, LOG_LOCAL0-7, LOG_LPR\n");
	printf("  LOG_MAIL, LOG_NEWS, LOG_SYSLOG, LOG_USER, LOG_UUCP\n");
	printf("valid values of levels:\n");
	printf("  LOG_EMERG, LOG_ALERT, LOG_CRIT, LOG_WARNNING, LOG_INFO, LOG_DEBUG\n"); 
	printf("\n");
}

void print_simple_usage(char *prog) {
	printf("\n");
	printf("write message \"msg\" to rsyslogd.\n");
	printf("usage: %s [-f facility] [-l level] msg\n", prog);
	printf("   -f facility		to specify what type of program is logging the message.\n");
	printf("   -l level             level of importance of the message.\n");
	printf("\n");
}

void usage(int argc, char *argv[], int type) {
	char *prog = strrchr(argv[0], '/');
	if(prog != NULL)
		prog++;
	else
		prog = argv[0];
	switch(type) {
	case USAGE_FULL:
		print_full_usage(prog);
		break;
	case USAGE_SIMPLE:
		print_simple_usage(prog);
		break;
	default:
		break;
	}
}

void init(int argc, char *argv[]) {
	char opt;
	int idx = 0, value;
	struct option opts[] = {
	  {"facility", required_argument, NULL, 'f'},
	  {"level", required_argument, NULL, 'l'},
	  {"help", no_argument, NULL, 'h'}
	};
	while((opt = getopt_long(argc, argv, "f:l:h", opts, &idx)) != -1) {
		switch(opt) {
		case 'f':
			value = name_to_value(facilities, optarg);
			if(value == NAME_ERROR) {
				printf("invalid facilite value. please use -h to get help information.\n");
				exit(1);
			}
			facility = value;
			break;
		case 'l':
			value = name_to_value(levels, optarg);
			if(value == NAME_ERROR) {
				printf("invalid level value. please use -h to get help information.\n");
				exit(1);
			}
			level = value;
			break;
		case 'h':
			usage(argc, argv, USAGE_FULL);
			exit(1);
		default:
			break;
		}
	}
	message = argv[optind];
	if(message == NULL) {
		usage(argc, argv, USAGE_SIMPLE);
		exit(1);
	}
}

void main(int argc, char *argv[]) {
	init(argc, argv);
	syslog(facility|level, "%s", message);
}
#endif
