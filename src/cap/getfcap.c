#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/capability.h>

extern yk_hexdump(const char *, int);

void main(int argc, char *argv[]){
	if(argc < 2) return;
	cap_t fcap = cap_get_file(argv[1]);
	if(fcap == NULL){
		printf("error: cap_get_file: %s\n", strerror(errno));
		return;
	}
	
	char *captext = cap_to_text(fcap, NULL);
	if(captext) {
		printf("cap-text: %s\n", captext);
	}
	
	yk_hexdump((const char*)fcap, cap_size(fcap));
	/*
	cap_value_t capval;
	cap_from_name("cap_sys_chroot", &capval);
	printf("cap_sys_chroot: %d\n", capval);
	cap_from_name("cap_chown", &capval);
	printf("cap_chown: %d\n", capval);
	
	if(cap_set_flag(fcap, CAP_EFFECTIVE, 1, &capval, CAP_CLEAR)){
		printf("error: cap_set_flag: %s\n", strerror(errno));
	}
	if(cap_set_flag(fcap, CAP_PERMITTED, 1, &capval, CAP_CLEAR)){
		printf("error: cap_set_flag: %s\n", strerror(errno));
	}
	
	printf("effecitve: %#x   permitted: %#x   inheritable: %#x\n", 
		CAP_EFFECTIVE, CAP_PERMITTED, CAP_INHERITABLE);
	
	yk_hexdump((const char*)fcap, cap_size(fcap));
	if(cap_set_file(argv[1], fcap)){
		printf("error: cap_set_file: %s\n", strerror(errno));
	}
	*/
	
exit_free:
	if(captext)
		cap_free(captext);

	if(fcap)
		cap_free(fcap);
}
