#include <stdio.h>
#include <stdlib.h>
#include <security/pam_modules.h>

void static echo_routine(int argc, const char **argv) {
	int i = 0;
	fprintf(stdout, "argc=%d argv:--", argc);
	for(; i < argc; i++) {
		fprintf(stdout, "[%d]%s ", i, argv[i]);
	}
	fprintf(stdout, "--\n");
}

int pam_sm_authenticate(pam_handle_t *h, int flags, int argc, const char **argv) {
	int ret;
#if 1 
	struct pam_conv *conv;
	ret = pam_get_item(h, PAM_CONV, (const void **)&conv);
	if(ret != PAM_SUCCESS) {
		fprintf(stderr, "error: pam_sm_authenticate: pam_get_item[PAM_CONV]: %s\n", pam_strerror(h, ret));
		return PAM_AUTH_ERR;
	}
	struct pam_message msgs[2] = {
		{ PAM_TEXT_INFO, "please intput password:"},
		{ PAM_PROMPT_ECHO_OFF, "\npassword:"}
	};
	struct pam_message *msg[2] = {
		(struct pam_message *)&(msgs[1]),
		(struct pam_message *) &(msg[0]),
	};
	//msg.msg_style = PAM_PROMPT_ECHO_OFF;
	//const struct pam_message **p = (struct pam_message **)&msgs;
#define MSG_NUM 1 
	struct pam_response *resps;
	//ret = conv->conv(MSG_NUM, (const struct pam_message **)&msgs, &resps, conv->appdata_ptr);
	//ret = conv->conv(2, p, &resps, conv->appdata_ptr);
	ret = conv->conv(MSG_NUM, (const struct pam_message **)&msg, &resps, conv->appdata_ptr);
	if(ret != PAM_SUCCESS) {
		fprintf(stderr, "error: conv: %s\n", pam_strerror(h, ret));
		return PAM_AUTH_ERR;
	}
	int i = 0;
	for(; i < MSG_NUM; i++) {
		if(resps && resps[i].resp) {
			printf("%d: %s\n", i, resps[i].resp);
			free(resps[i].resp);
			resps->resp = NULL;
		}
	}
	if(resps) {
		free(resps);
		resps = NULL;
	}
#endif
printf("pam_sm_authenticate: ", argv[0]);
	echo_routine(argc, argv);;
	return  PAM_SUCCESS;
}

int pam_sm_acct_mgmt(pam_handle_t *h, int flags, int argc, const char **argv) {
printf("pam_sm_acct_mgmt: ");
	echo_routine(argc, argv);;
	return PAM_SUCCESS;
}

int pam_sm_open_session(pam_handle_t *h, int flags, int argc, const char **argv) {
printf("pam_sm_open_session: ");
	echo_routine(argc, argv);;
	return PAM_SUCCESS;
}

int pam_sm_close_session(pam_handle_t *h, int flags, int argc, const char **argv) {
printf("pam_sm_close_session: ");
	echo_routine(argc, argv);;
	return PAM_SUCCESS;
}

int pam_sm_setcred(pam_handle_t *h, int flags, int argc, const char **argv) {
printf("pam_sm_setcred: ");
	echo_routine(argc, argv);;
	return PAM_SUCCESS;
}

int pam_sm_chauthtok(pam_handle_t *h, int flags, int argc, const char **argv) {
printf("pam_sm_chautok: ");
	echo_routine(argc, argv);;
	return PAM_SUCCESS;
}
