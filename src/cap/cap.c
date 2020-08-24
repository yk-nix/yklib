#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/capability.h>

extern yk_hexdump(const char *, int);

#ifdef _DEBUG_CAP_
void main(int argc, char *argv[]){
	cap_t cap_self;
	cap_self = cap_get_proc();
	if(cap_self == NULL) {
		printf("error: cap_get_file: %s: %s\n", argv[1], strerror(errno));
		goto exit;
	}
	printf("capabilities of myself: %s\n", cap_to_text(cap_self, NULL));
	
	cap_t cap;
	char * file = argv[1];
	if(argc < 2)
		goto exit;
	cap = cap_get_file(file);
	if(cap == NULL) {
		printf("error: cap_get_file: %s: %s\n", file, strerror(errno));
	}
	if(cap)
		printf("capbilities of %s: %s\n", file, cap_to_text(cap, NULL));
	else
		cap = cap_init();
	ssize_t size = cap_size(cap);
	void *buf = malloc(size);
	if(buf == NULL) {
		printf("error: malloc: %s\n", strerror(errno));
	}
	memset(buf, 0, size);
	if(cap_copy_ext(buf, cap, size) < 0) {
		printf("error: cap_copy_ext: %s\n", strerror(errno));
	}
	yk_hexdump(buf, size);
	
	/*
	int ret;
	cap_value_t caps[10] = { CAP_SETPCAP, CAP_SETFCAP };
	if(cap_set_flag(cap, CAP_EFFECTIVE, 2, caps, CAP_SET)) {
		printf("error: cap_set_flag: %s\n", strerror(errno));
		goto exit;
	}
	if(cap_set_flag(cap, CAP_PERMITTED, 2, caps, CAP_SET)) {
		printf("error: cap_set_flag: %s\n", strerror(errno));
		goto exit;
	}
	if(cap_set_file(file, cap)) {
		printf("error: cap_set_file: %s: %s\n", file, strerror(errno));
		goto exit;
	}
	*/

exit:
	if(cap_self)
		cap_free(cap_self);
	if(cap)
		cap_free(cap);
}
#endif
