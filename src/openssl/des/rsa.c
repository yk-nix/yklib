#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <openssl/rsa.h>
#include <openssl/err.h>
int main(int argc, char argv[]) {
	RSA *rsa = RSA_new();
	if(rsa == NULL) {
		ERR_print_errors_fp(stdout);
		return;
	}
	printf("RSA size: %d\n", RSA_size(rsa));

exit:
	if(rsa)
		RSA_free(rsa);
}
