#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

extern yk_hexdump(char *, int);

#define RAND_MODE_HEX			1
#define RAND_MODE_BASE64		2

static int bytes = 8;
static char *progname = NULL;
static int mode = RAND_MODE_HEX;

static void usage(int exit_code) {
	fprintf(stdout, "%s  [-Hbh] <bytes>\n", progname);
	fprintf(stdout, "generate a random number with its length of <bytes> bytes.\n");
	fprintf(stdout, "  -H hex       output the random number with hex format\n");
	fprintf(stdout, "  -b base64    outout the random number with base64 format\n");
	fprintf(stdout, "  -h help      display this help message\n");
	exit(exit_code);
}
static void init(int argc, char *argv[]) {
	int opt, idx;
	struct option opts[] = {
		{"base64", no_argument, NULL, 'b'},
		{"hex", no_argument, NULL, 'H'},
		{"help", no_argument, NULL, 'h'},
	};
	progname = strrchr(argv[0], '/');
	progname == NULL ? (progname = argv[0]) : (progname++);
	while((opt = getopt_long(argc, argv, "Hbh", opts, &idx)) != -1) {
		switch(opt) {
		case 'H':
			mode = RAND_MODE_HEX;
			break;
		case 'b':
			mode = RAND_MODE_BASE64;
			break;
		case 'h':
			usage(0);
			break;
		default:
			usage(1);
			break;
		}
	}
	if(argv[optind])
		bytes = atoi(argv[optind]);
}

static char *rand_generate(char *cache, int bytes) {
	if(RAND_bytes(cache, bytes))
		return cache;
	return NULL;	
}

static void rand_print_hex(char *cache, int bytes) {
	int i = 0;
	fprintf(stdout, "0x");
	for(; i < bytes; i++)
		fprintf(stdout, "%02x", (unsigned char )cache[i]);
	fprintf(stdout, "\n");
}

static void rand_print_base64(char *cache, int bytes) {
	BIO *out = BIO_new_fp(stdout, BIO_NOCLOSE);
	BIO *b64 = BIO_new(BIO_f_base64());
	BIO_push(b64, out);
	BIO_write(b64, cache, bytes);
	BIO_flush(b64);
	BIO_free_all(b64);
}

static void rand_print(char *cache, int bytes) {
	switch(mode) {
	case RAND_MODE_HEX:
		rand_print_hex(cache, bytes);
		break;
	case RAND_MODE_BASE64:
		rand_print_base64(cache, bytes);
		break;
	default:
		break;
	}
}
#include <openssl/engine.h>
#include <openssl/err.h>
#include "../engine/ykcrypto.h"
int main(int argc, char *argv[]) {
	SSL_load_error_strings();
	ENGINE_load_ykcrypto();
	ENGINE *e = ENGINE_by_id("ID_ykcrypto");
	if(!e) {
		fprintf(stderr, "no suche engine: ID_ykcrypto\n");
		ERR_print_errors_fp(stderr);
	}
	ENGINE_set_default_RAND(e);
	init(argc, argv);
	if(bytes <= 0)
		return 1;
	char *cache = (char *)calloc(1, bytes);
	if(cache == NULL) {
		fprintf(stderr, "memory allocation error: %m\n");
		return;
	}
	RAND_cleanup();
	if(!rand_generate(cache, bytes))
		ERR_print_errors_fp(stderr);
	rand_print(cache, bytes);
	free(cache);
	ENGINE_free(e);
	return 0;
}
