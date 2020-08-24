#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <yk_openssl.h>
#include <yk_utils.h>

int main(int argc, char *argv[]) {
	int ret = 1;
	if(argc < 2)
		return 1;
	char *err;
	int len = strtol(argv[1], &err, 0);
	if(len == 0 && err == argv[1]) {
		fprintf(stderr, "bad number of bytes: %s\n", argv[1]);
		return 1;
	}
	if(len <= 0)
		return 1;
	
	char *buf = (char *)calloc(1, len);
	if(buf == NULL)
		return 1;
	if(yk_rand_generate(buf, len) == NULL) {
		fprintf(stderr, "error to genernate %d bytes of random data\n");
		goto error;
	}
		
	yk_hexdump(buf, len);
error:
	free(buf);
	return ret;
}
