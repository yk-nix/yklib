
#include <string.h>
#include <openssl/rand.h>

static void seed(const void *buf, int num) {
}
static int bytes(unsigned char *buf, int num) {
	int i = 0;
	while(i < num)
		memset(buf++, i++, 1);
}
static void cleanup(void) {
}
static void add(const void *buf, int num, double entropy) {
}
static int pseudorand(unsigned char *buf, int num) {
	return -1;
}
static int status(void) {
	return -1;
}

RAND_METHOD ykrand = {
	.seed    = seed,
	.bytes   = bytes,
	.cleanup = cleanup,
	.add     = add,
	.status  = status,
	.pseudorand = pseudorand,
};
