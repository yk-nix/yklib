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

static char *progname = NULL;
static int  mode = BN_HEX;

void dump_args(int argc, char *argv[]) {
	int i = 0;
	for(; i < argc; i++)
		fprintf(stdout, "%02d   %s\n", i, argv[i]);
}

void usage(int ret_code) {
	fprintf(stdout, "usage:  %s [-hd] <n> <e>\n", progname);
	fprintf(stdout, "Calculate the value:(n^e). All the numbers must be specified as hexes,\n");
        fprintf(stdout, "unless you specified the '-d' option to use decimal numbers.\n");
	fprintf(stdout, "   -d          use decimal number instead of hex.\n");
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
	while((opt = getopt_long(argc, argv, "hd", opts, &idx)) != -1) {
		switch(opt) {
		case 'd':
			mode = BN_DEC;
			break;
		case 'h':
			usage(0);
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
        switch(mode) {
        case BN_HEX:
                return BN_hex2bn(n, s);
        case BN_DEC:
                return BN_dec2bn(n, s);
        default:
                return -1;
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
	
	int ret = 1, i;	

	init(argc, argv);
	
	SSL_load_error_strings();
	BN_CTX *ctx = BN_CTX_new();
	if(ctx == NULL)
		goto err;
	BN_CTX_start(ctx);
	BIGNUM *v, *n, *e;
	v = BN_CTX_get(ctx);
	n = BN_CTX_get(ctx);
	e = BN_CTX_get(ctx);
	if(v == NULL || n == NULL || e == NULL) {
		ERR_print_errors_fp(stderr);
		goto err;
	}
	if(str2bn(&n, argv[optind]) == BN_ERROR) {
		ERR_print_errors_fp(stderr);
		goto err;
	}
	if(str2bn(&e, argv[optind+1]) == BN_ERROR) {
		ERR_print_errors_fp(stderr);
		goto err;
	}
	if(BN_exp(v, n, e, ctx) != BN_SUCCESS) {
		ERR_print_errors_fp(stderr);
		goto err;
	}
	char *str = bn2str(v);
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

