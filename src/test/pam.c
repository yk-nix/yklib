#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <security/pam_appl.h>
#include <security/pam_misc.h>

static int yk_misc_conv(int num_msg, const struct pam_message **msgm, struct pam_response **responese, void *appdata_ptr) {
	int count = 0;
	struct pam_response *reply;

	if (num_msg <= 0)
		return -1;

	reply = (struct pam_response *) calloc(num_msg, sizeof(struct pam_response));
	if(reply == NULL) {
		printf("no memmoery for responses\n");
		return -1;
	}
	for(count = 0; count < num_msg; ++count) {
		switch (msgm[count]->msg_style) {
		case PAM_PROMPT_ECHO_OFF:
			break;
		case PAM_PROMPT_ECHO_ON:
			break;
		case PAM_ERROR_MSG:
			break;
		case PAM_TEXT_INFO:
			break;
		default:
		break;
		}
	}
printf("conv return\n");
	return PAM_SUCCESS;
}

struct pam_conv conv = {
	misc_conv,     /* use the general line coversation */
	NULL           /* no application data, so set it to be NULL */
};

int main(int argc, char *argv[]) {
	pam_handle_t *h;
	int ret;
	ret = pam_start("yoka",     /* service name */
			NULL,       /* user name, not used here, so we set it as NULL */
			&conv,  /* how to conversation with end user */
			&h);        /* pam handle */ 
	if(ret != PAM_SUCCESS) {
		fprintf(stderr, "error: pam_start: %s\n", pam_strerror(h, ret));
		return 1;
	}
	
	/*
	if((ret = pam_set_item(h, PAM_RHOST, "HelloWorld")) != PAM_SUCCESS) {
		fprintf(stderr, "error: pam_set_item[PAM_RHOST]: %s\n", pam_strerror(h, ret));
		goto _err0; 
	}
	*/

	
	ret = pam_authenticate(h,  /* pam handle */
			       0); /* such flags as: PAM_SILENT, PAM_DISALLOW_NULL_AUTHTOK */
	if(ret != PAM_SUCCESS) {
		fprintf(stderr, "error: authenticate error: %s\n", pam_strerror(h, ret));
		goto _err0;
	}
	fprintf(stdout, "authentication: OK\n");
	ret = pam_acct_mgmt(h, 0);
	if(ret != PAM_SUCCESS) {
		fprintf(stderr, "error: pam_acct_mgmt error: %s\n", pam_strerror(h, ret));
		goto _err0;
	}
	fprintf(stdout, "account check: OK\n");

_err0:
	ret = pam_end(h, 0);
	if(ret != PAM_SUCCESS)
		fprintf(stderr, "error: pam_end: %s\n", pam_strerror(h, ret));

	return 1;
}
