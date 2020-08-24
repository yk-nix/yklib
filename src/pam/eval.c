#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>

#include <pwd.h>
#include <getopt.h>

#include <security/pam_appl.h>
#include <security/pam_misc.h>


static int debug = 0;
static char *progname = NULL;
static char *username = NULL;
static char *password = NULL;
static char *cmd = NULL;

static struct pam_conv pam_conversation = {
	.conv        = misc_conv,
	.appdata_ptr = NULL,
};

void debug_printf(char *fmt, ...) {
	if(debug) {
		va_list ap;
		va_start(ap, fmt);
		vprintf(fmt, ap);
		va_end(ap);
	}
}

static int do_verify(void) {
	int ret = 0;
	pam_handle_t *pamh;
	
	ret = pam_start(progname, username, &pam_conversation, &pamh);
	if(ret != PAM_SUCCESS) {
		debug_printf("error: pam_start: %s\n", pam_strerror(pamh, ret));
		goto error_exit;		
	}
	/* PAM_SILENT, PAM_DISALLOW_NULL_AUTHTOK */
	ret = pam_authenticate(pamh, PAM_DISALLOW_NULL_AUTHTOK);
	if(ret != PAM_SUCCESS) {
		debug_printf("error: pam_authenticate: %s\n", pam_strerror(pamh, ret));
		goto error_exit;
	}
	ret = pam_acct_mgmt(pamh, 0);
	if(ret != PAM_SUCCESS) {
		debug_printf("error: pam_acct_mgmt: %s\n", pam_strerror(pamh, ret));
		goto error_exit;
	}
	ret = 1;
	goto exit;
error_exit:
	ret = 0;
exit:
	if(pamh)
		pam_end(pamh, 0);
	return ret;
}

void init(int argc, char *argv[]) {
	uid_t uid = getuid();
	struct passwd *pw = getpwuid(uid);
	if(pw)
		username = pw->pw_name;
	
	progname = strrchr(argv[0], '/');
	if(progname != NULL)
		progname++;
	else
		progname = argv[0];
}

void parse_options(int argc, char *argv[]) {
	char opt;
	struct option opts[] = {
		{"user", required_argument, NULL, 'u'},
		{"password", required_argument, NULL, 'p'},
		{"debug", no_argument, NULL, 'D'},
		{"help", no_argument, NULL, 'h'},
	};
	int idx = 0;
	while((opt = getopt_long(argc, argv, "u:p:Dh", opts, &idx)) != -1) {
		switch(opt) {
		case 'u':
			username = optarg;
			break;
		case 'p':
			password = optarg;
			break;
		case 'D':
			debug = 1;
			break;
		case 'h':
			exit(0);
		default:
			break;
		}
	}
	cmd = argv[optind];
	if(!cmd)
		exit(1);
}

int main(int argc, char *argv[]) {
	init(argc, argv);
	parse_options(argc, argv);
	debug_printf("user: %s     password:%s    cmd: %s\n", username, password, cmd);
	if(do_verify()) {
		return system(cmd);
	}
	return -1;
}
