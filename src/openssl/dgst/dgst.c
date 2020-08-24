#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>
#include <openssl/evp.h>

/* the default message digest algorithm is md5 */

static const EVP_MD	*md;
static FILE		*file;


void init(int argc, char *argv[]) {
	char *dgst_name = "md5";
	SSL_load_error_strings();
	OpenSSL_add_all_digests();
	char opt;
	while((opt = getopt(argc, argv, "-a:")) != -1) {
		switch(opt) {
		case 'a':
			dgst_name = optarg;
			break;
		case 1:
			file = fopen(optarg, "rb");
			if(file == NULL)  {
				printf("open file error: %s\n", strerror(errno));
				goto err;
			}
			printf("file: %s\n", optarg);
			break;
		default:
			break;
		}
	}
	md = EVP_get_digestbyname(dgst_name);
	if(md == NULL) {
		printf("unkown message digest type: %s\n", dgst_name);
		goto err;
	}
	printf("md: %s\n", dgst_name);
	if(file == NULL) {
		printf("please specify a file to do md\n");
	}
	return;
err:
	EVP_cleanup();
	exit(1);
}

void dispose(void *arg) {
	EVP_cleanup();
	if(file)
		fclose(file);
}

void main(int argc, char *argv[]) {
	init(argc, argv);
	dispose(NULL);
}
