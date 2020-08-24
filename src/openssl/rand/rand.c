
#include <stdio.h>
#include <openssl/rand.h>

char *yk_rand_generate(char *buf, size_t size) {
	if(RAND_bytes(buf, size))
		return buf;
	return NULL;
}
