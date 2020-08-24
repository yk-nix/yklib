#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include <openssl/bn.h>
#include <openssl/bio.h>
#include <openssl/err.h>

#define BN_SUCCESS	1
#define BN_ERROR	0
#define BN_DEC		0x0001
#define BN_SIMPLE	0x0002

static char *progname = NULL;
static int  mode = 0;

void dump_args(int argc, char *argv[]) {
	int i = 0;
	for(; i < argc; i++)
		fprintf(stdout, "%02d   %s\n", i, argv[i]);
}

void usage(int ret_code) {
	fprintf(stdout, "usage:  %s [-hds] <n> <d>\n", progname);
	fprintf(stdout, "Calculate the result and remainder of (ret=n/d rem=n%%d). All the numbers must be specified as hexes,\n");
        fprintf(stdout, "unless you specified the '-d' option to use decimal numbers.\n");
	fprintf(stdout, "   -d          use decimal number instead of hex.\n");
	fprintf(stdout, "   -s simple   output with simple mode.(no remainder if its 0)\n");
	fprintf(stdout, "   -h help     display this help message.\n");
	exit(ret_code);
}

void init(int argc, char *argv[]) {
	progname = strrchr(argv[0], '/');
	if(progname == NULL)
		progname = argv[0];
	else
		progname++;

	char opt;
	struct option opts[] = {
		{"help", no_argument, NULL, 'h'},
	};
	int idx = 0;
	while((opt = getopt_long(argc, argv, "hds", opts, &idx)) != -1) {
		switch(opt) {
		case 'd':
			mode |= BN_DEC;
			break;
		case 'h':
			usage(0);
			break;
		case 's':
			mode |= BN_SIMPLE;
			break;
		default:
			usage(1);
			break;
		}
	}
	if(argv[optind] == NULL || argv[optind+1] == NULL)
		usage(1);
}

inline static int str2bn(BIGNUM **n, char *s) {
	if(mode & BN_DEC)
		return BN_dec2bn(n, s);
	return BN_hex2bn(n, s);
}

inline static char *bn2str(BIGNUM *n) {
	if(mode & BN_DEC)
		return BN_bn2dec(n);
	return BN_bn2hex(n);
}

int main(int argc, char *argv[]) {
	
	int ret = 1, i;	

	init(argc, argv);
	
	SSL_load_error_strings();
	BN_CTX *ctx = BN_CTX_new();
	if(ctx == NULL)
		goto err;
	BN_CTX_start(ctx);
	BIGNUM *v, *n, *d, *r;
	v = BN_CTX_get(ctx);
	n = BN_CTX_get(ctx);
	d = BN_CTX_get(ctx);
	r = BN_CTX_get(ctx);
	if(v == NULL || n == NULL || d == NULL || r == NULL) {
		ERR_print_errors_fp(stderr);
		goto err;
	}
	if(str2bn(&n, argv[optind]) == BN_ERROR) {
		ERR_print_errors_fp(stderr);
		goto err;
	}
	if(str2bn(&d, argv[optind+1]) == BN_ERROR) {
		ERR_print_errors_fp(stderr);
		goto err;
	}
	if(BN_div(v, r, n, d, ctx) != BN_SUCCESS) {
		ERR_print_errors_fp(stderr);
		goto err;
	}
	char *vstr = bn2str(v);
	char *rstr = bn2str(r);
	if(vstr) {
		if(mode & BN_SIMPLE)
			fprintf(stdout, "%s\n", vstr);
		else
			fprintf(stdout, "result:   %s\n", vstr);
		OPENSSL_free(vstr);
	}
	if(rstr) {
		if(mode & BN_SIMPLE) {
			if(!BN_is_zero(r))
				fprintf(stdout, "%s\n", rstr);
		}
		else
			fprintf(stdout, "remainder: %s\n", rstr);
		OPENSSL_free(rstr);
		ret = 0;
	}
err:
	if(ctx) {
		BN_CTX_end(ctx);
		BN_CTX_free(ctx);
	}
	return ret;
}

