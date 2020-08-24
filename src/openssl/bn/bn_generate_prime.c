#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include <openssl/bn.h>
#include <openssl/bio.h>
#include <openssl/err.h>

#define BN_SUCCESS	1
#define BN_ERROR	0
#define BN_HEX		1
#define BN_DEC		2


static int numbits = 1024;
static int safe    = 0;
static char *progname = NULL;
static int mode = BN_HEX;

void usage(int ret_code) {
	fprintf(stdout, "%s [-shd] [numbits]\n", progname);
	fprintf(stdout, "   -d          output the number instead of hex whth decimal format\n");
	fprintf(stdout, "   -s safe     generate a safe prime number p(that (p-1)/2 is aslo prime)\n");
	fprintf(stdout, "   -h help     display this help message\n");
	exit(ret_code);
}

void init(int argc, char *argv[]) {
	int num;
	char opt;
	struct option opts[] = {
		{"safe", no_argument, NULL, 's'},
		{"help", no_argument, NULL, 'h'},
	};
	progname = strrchr(argv[0], '/');
	if(progname == NULL)
		progname = argv[0];
	else
		progname++;
	while((opt = getopt_long(argc, argv, "shd", opts, &num)) != -1) {
		switch(opt) {
		case 's':
			safe = 1;
			break;
		case 'h':
			usage(0);
			break;
		case 'd':
			mode = BN_DEC;
			break;
		default:
			usage(1);
			break;
		}
	}
	if(argv[optind]) {
		num = atoi(argv[optind]);
		if(num > 0)
			numbits = num;
	}
}

inline static char *bn2str(BIGNUM *n) {
	switch(mode) {
	case BN_HEX:
		return BN_bn2hex(n);
	case BN_DEC:
		return BN_bn2dec(n);
	default:
		return NULL;
	}
}

int main(int argc, char *argv[]) {
	
	int ret = 1;	

	init(argc, argv);

	SSL_load_error_strings();
	BN_CTX *ctx = BN_CTX_new();
	if(ctx == NULL)
		goto err;
	BN_CTX_start(ctx);
	BIGNUM *p = BN_CTX_get(ctx);
	if(BN_generate_prime_ex(p, numbits, safe, NULL, NULL, NULL) != BN_SUCCESS) {
		ERR_print_errors_fp(stderr);
		goto err;
	}
	char *str = bn2str(p);
	if(str) {
		fprintf(stdout, "%s\n", str);
		OPENSSL_free(str);
		ret = 0;
	}
err:
	if(ctx) {
		BN_CTX_end(ctx);
		BN_CTX_free(ctx);
	}
	return ret;
}

