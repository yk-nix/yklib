#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <openssl/ssl.h>
#include <openssl/pem.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void main(int argc, char *argv[]) {
	/* to user ssl library, we must initialize this libaray at the very first */
	SSL_load_error_strings();
	//ERR_load_crypto_strings();
	SSL_library_init();
	//OpenSSL_add_all_algorithms();
	
	/* create a SSL/TLS working framework */
	SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());
	if(!ctx) {
		printf("error: SSL_CTX_new:\n");
		goto exit;
	}
	OpenSSL_add_all_algorithms();
	//SSL_CTX_set_cipher_list(ctx, "ECDHE-ECDSA-AES256-GCM-SHA384");
	SSL *ssl = SSL_new(ctx);
	printf("cipher list: %s\n", SSL_get_cipher_list(ssl, 0));
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0) {
		printf("socket creating: error: %s\n", strerror(errno)); 
		goto exit;
	}
	if(argc < 3)
		return;
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(atoi(argv[2])),
		.sin_addr = inet_addr(argv[1]),
	};
	int bytes = 0;
	char buf[1024];
	connect(sock, (struct sockaddr *)&addr, sizeof(addr));
	SSL_set_fd(ssl, sock);
	printf("hello1\n");
	if(SSL_connect(ssl) == -1) {
		ERR_print_errors_fp(stdout);
		printf("hello3\n");
		goto sock_exit;
	}
	printf("hello2\n");
	while(1) {
		memset(buf, 0, sizeof(buf));
		scanf("%s", buf);
		bytes = SSL_write(ssl, buf, strlen(buf));
		if(bytes <= 0) {
			ERR_print_errors_fp(stdout);
			break;
		}
	}
	SSL_shutdown(ssl);
sock_exit:
	close(sock);
exit:
	if(ssl)
		SSL_free(ssl);
	if(ctx)
		SSL_CTX_free(ctx);
		
}
