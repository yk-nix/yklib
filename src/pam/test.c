#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <security/pam_appl.h>
#include <security/pam_misc.h>

struct pam_conv pam_conversation = {
	.conv = misc_conv,
	.appdata_ptr = "mydata",
};

void main(int argc, char *argv[]) {
	pam_handle_t *pamh;
	char *service_name = "mytest";
	char *user = "yoka";
	int ret = 0;
	
	if(argc > 3) {
		printf("usage: %s [username [service-name]]\n", argv[0]);
	}
	if((argc >=2) && (argv[1][0] != '-')) {
		user = argv[1];		
	}
	if(argc == 3) {
		service_name = argv[2];
	}
	
	ret = pam_start(service_name, user, &pam_conversation, &pamh);
	if(ret != PAM_SUCCESS) {
		printf("error: pam_start: %d", ret);
		return;
	}
	char *service;
	char *username;
	ret = pam_get_item(pamh, PAM_USER, (const void **)&username);
	if(ret != PAM_SUCCESS) {
		printf("error: pam_get_item[PAM_USER]: %s\n", pam_strerror(pamh, ret));
	}
	else {
		printf("user: %s\n", username);
	}
	ret = pam_set_item(pamh, PAM_SERVICE, service_name);
	if(ret != PAM_SUCCESS) {
		printf("error: pam_set_item[PAM_SERVICE]: %s\n", pam_strerror(pamh, ret));
	}
	ret = pam_get_item(pamh, PAM_SERVICE, (const void **)&service);
	if(ret != PAM_SUCCESS) {
		printf("error: pam_get_item[PAM_SERIVCE]: %s\n", pam_strerror(pamh, ret));
	}	
	else {
		printf("service: %s\n", service);
	}

	printf("pam_conv addr: %#x\n", &pam_conversation);
	const struct pam_conv *conversation = NULL;
	ret = pam_get_item(pamh, PAM_CONV, (const void **)&conversation);
	if(ret != PAM_SUCCESS) {
		printf("error: pam_get_item[PAM_CONV]: %s\n", pam_strerror(pamh, ret));
	}	
	else {
		printf("appdata: %s\n", (char *)conversation->appdata_ptr);
	}

	const char *user_prompt;
	ret = pam_get_item(pamh, PAM_USER_PROMPT, (const void **)&user_prompt);
	if(ret != PAM_SUCCESS) {
		printf("error: pam_get_item[PAM_USER_PROMPT]: %s\n", pam_strerror(pamh, ret));
	}	
	else {
		printf("user_prompt: %s\n", user_prompt);
	}
	
	const char *tty;
	ret = pam_get_item(pamh, PAM_TTY, (const void **)&tty);
	if(ret != PAM_SUCCESS) {
		printf("error: pam_get_item[PAM_TTY]: %s\n", pam_strerror(pamh, ret));
	}	
	else {
		printf("tty: %s\n", tty);
	}
	
	const char *ruser;
	ret = pam_get_item(pamh, PAM_RUSER, (const void **)&ruser);
	if(ret != PAM_SUCCESS) {
		printf("error: pam_get_item[PAM_RUSER]: %s\n", pam_strerror(pamh, ret));
	}	
	else {
		printf("ruser: %s\n", ruser);
	}
	
	const char *rhost;
	ret = pam_get_item(pamh, PAM_RHOST, (const void **)&rhost);
	if(ret != PAM_SUCCESS) {
		printf("error: pam_get_item[PAM_RHOST]: %s\n", pam_strerror(pamh, ret));
	}	
	else {
		printf("rhost: %s\n", rhost);
	}
	
	const char *authtok;
	ret = pam_get_item(pamh, PAM_AUTHTOK, (const void **)&authtok);
	if(ret != PAM_SUCCESS) {
		printf("error: pam_get_item[PAM_AUTHOK]: %s\n", pam_strerror(pamh, ret));
	}	
	else {
		printf("authok: %s\n", authtok);
	}
	
	const char *oldauthtok;
	ret = pam_get_item(pamh, PAM_OLDAUTHTOK, (const void **)&oldauthtok);
	if(ret != PAM_SUCCESS) {
		printf("error: pam_get_item[PAM_OLDAUTHTOK]: %s\n", pam_strerror(pamh, ret));
	}	
	else {
		printf("oldauthtok: %s\n", oldauthtok);
	}
	
	for(;;) {
		//ret = pam_authenticate(pamh, PAM_DISALLOW_NULL_AUTHTOK|PAM_SILENT);
		ret = pam_authenticate(pamh, PAM_DISALLOW_NULL_AUTHTOK);
		if(ret != PAM_SUCCESS) {
			printf("error: pam_authenticate: %s\n", pam_strerror(pamh, ret));
			break;
		}
		printf("authenticate: OK\n");
		
		ret = pam_acct_mgmt(pamh, PAM_DISALLOW_NULL_AUTHTOK);
		if(ret != PAM_SUCCESS) {
			printf("error: pam_acct_mgmt: %s\n", pam_strerror(pamh, ret));
			break;
		}
		printf("account check: OK\n");
		
		ret = pam_setcred(pamh, PAM_ESTABLISH_CRED);
		if(ret != PAM_SUCCESS) {
			printf("error: pam_setcred[PAM_ESTABLISH_CRED]: %s\n", pam_strerror(pamh, ret));
			break;
		}
		printf("cred established: OK\n");

		ret = pam_open_session(pamh, 0);
		if(ret != PAM_SUCCESS) {
			printf("error: pam_open_session: %s\n", pam_strerror(pamh, ret));
			break;
		}
		printf("open session: OK\n");
		
		ret = pam_close_session(pamh, 0);
		if(ret != PAM_SUCCESS) {
			printf("error: pam_close_session: %s\n", pam_strerror(pamh, ret));
			break;
		}
		printf("close session: OK\n");
		
		ret = pam_setcred(pamh, PAM_DELETE_CRED);
		if(ret != PAM_SUCCESS) {
			printf("error: pam_setcred[PMA_DELETE_CRED]: %s\n", pam_strerror(pamh, ret));
			break;
		}
		printf("cred deleted: OK\n");
		break;
	}
	pam_end(pamh, 0);
}
