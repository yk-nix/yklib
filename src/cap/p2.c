#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/capability.h>

extern yk_hexdump(const char *, int);

void main(int argc, char *argv[]){
	cap_t cap = cap_get_proc();
	yk_hexdump((const char *)cap, cap_size(cap));
	
	
exit_free:
	if(cap)
		cap_free(cap);
}
