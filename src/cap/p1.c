#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/capability.h>

extern yk_hexdump(const char *, int);

void main(int argc, char *argv[]){
	cap_t cap = cap_get_proc();
	cap_value_t cval = CAP_CHOWN;
	if(cap_set_flag(cap, CAP_INHERITABLE, 1, &cval, CAP_SET)){
		printf("error: cap_set_flag: %s\n", strerror(errno));
		goto exit_free;
	}
	if(cap_set_proc(cap)){
		printf("error: cap_set_proc: %s\n", strerror(errno));
		goto exit_free;
	}
	yk_hexdump((const char *)cap, cap_size(cap));
	if(argc > 1){
		execlp(argv[1], argv[1], NULL);
		printf("error: execlp: %s\n", strerror(errno));
	}
	
exit_free:
	if(cap)
		cap_free(cap);
}
