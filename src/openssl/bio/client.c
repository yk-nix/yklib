#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define SSL_ERROR       0
#define SSL_SUCCESS     1

struct inetent {
	char	*name;
	char	*port;
};

static struct inetent hostname = {
	.name = "localhost",
	.port = "1314",
};

static void usage(int ret) {
	exit(ret);
}

static struct inetent *str2inetent(char *s, struct inetent *ent) {
	char *coma = strrchr(s, ':');
	if(coma)
		ent->port = coma + 1;
	ent->name = s;
	return ent;
}

static char *inetent2str(struct inetent *ent, char *s, int *len) {
	if(*len <= 0)
		return NULL;
	int n = snprintf(s, *len, "%s:%s", ent->name, ent->port);
	if(*len <= n) {
		memset(s, 0, *len);
		return NULL;
	}
	*len = n;
	return s;
}

static void init(int argc, char *argv[]) {
	char opt;
	struct option opts[] = {
		{"help", no_argument, NULL, 'h'}
	};
	int idx = 0;
	while((opt = getopt_long(argc, argv, "h", opts, &idx)) != -1) {
		switch(opt) {
		case	'h':
			usage(0);
		default:
			usage(1);
		}
	}
	if(argv[optind] == NULL) 
		usage(1);
	str2inetent(argv[optind], &hostname);
}

int main(int argc, char *argv[]) {
	char buf[1024], *remotehost;
	int ret = 1, len = sizeof(buf);
	
	init(argc, argv);
	memset(buf, 0, sizeof(buf));
	remotehost = inetent2str(&hostname, buf, &len);
	if(remotehost == NULL) {
		fprintf(stderr, "the hostname is too long\n");
		goto err0;
	}
	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();
	SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());
	if(ctx == NULL) {
		ERR_print_errors_fp(stderr);
		goto err0;
	}
	BIO *ssl   = BIO_new_ssl(ctx, 1);
	BIO *conn  = BIO_new_connect(remotehost);
	if(ssl == NULL || conn == NULL) {
		ERR_print_errors_fp(stderr);
		goto err1;
	}
	BIO_push(ssl, conn);
	if(BIO_do_connect(conn) != SSL_SUCCESS) {
		ERR_print_errors_fp(stderr);
		goto err2;
	}
	if(BIO_do_handshake(ssl) != SSL_SUCCESS) {
		ERR_print_errors_fp(stderr);
		goto err2;
	}
	while(1) {
		memset(buf, 0, sizeof(buf));
		scanf("%s", buf);
		if(strcasecmp(buf, "exit") == 0)
			break;
		BIO_write(ssl, buf, strlen(buf));
	}
err2:
	BIO_free_all(ssl);
err1:
	if(ctx) {
		SSL_CTX_free(ctx);
	}
err0:
	return ret;
}

