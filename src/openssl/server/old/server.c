#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <openssl/ssl.h>
#include <openssl/pem.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int pem_password(char *buf, int size, int rwflag) {
	size = sprintf(buf, "yoka");
	return size;
}

void main(int argc, char *argv[]) {
	/* to user ssl library, we must initialize this libaray at the very first */
	SSL_library_init();
	//OpenSSL_add_all_algorithms();
	SSL_load_error_strings();
	
	/* create a SSL/TLS working framework */
	SSL_CTX *ctx = SSL_CTX_new(SSLv23_server_method());
	if(!ctx) {
		printf("error: SSL_CTX_new:\n");
		goto exit;
	}
	SSL_CTX_set_default_passwd_cb_userdata(ctx, "yoka");
	//SSL_CTX_set_cipher_list(ctx, "ECDHE-ECDSA-AES256-GCM-SHA384");
	//SSL_CTX_set_default_passwd_cb(ctx, pem_password);
	SSL *ssl = SSL_new(ctx);
	SSL_use_certificate_file(ssl, "/home/yui/tmp/openssl/demoCA/req/rsa512.crt", SSL_FILETYPE_PEM);
	SSL_use_PrivateKey_file(ssl, "/home/yui/tmp/openssl/demoCA/private/rsa/512.pem", SSL_FILETYPE_PEM);
	printf("prvaite key check...            %s\n", SSL_check_private_key(ssl) ? "OK" : "NG");
	printf("cipher list: %s\n", SSL_get_cipher_list(ssl, 0));
	
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0) {
		printf("socket creating: error: %s\n", strerror(errno)); 
		goto exit;
	}
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(1234),
		.sin_addr = INADDR_ANY,
	};
	if(bind(sock, (struct sockaddr *)&addr, sizeof(addr))) {
		printf("socket binding: error: %s\n", strerror(errno));
		goto sock_exit;
	}
	if(listen(sock, 10)) {
		printf("socket listening: error: %s\n", strerror(errno));
		goto sock_exit;
	}
	int bytes = 0;
	char buf[1024];
	while(1) {
		struct sockaddr_in remote_addr;
		int len = sizeof(remote_addr);
		int s = accept(sock, (struct sockaddr *)&remote_addr, &len);
		SSL_set_fd(ssl, s);
		if(SSL_accept(ssl) == -1) {
			ERR_print_errors_fp(stdout);
			break;
		}
		printf("ssl handshake: ok\n");
		while(1) {
			memset(buf, 0, sizeof(buf));
			bytes = SSL_read(ssl, buf, sizeof(buf));
			if(bytes > 0) {
				printf("%s", buf);
			}
			else {
				ERR_print_errors_fp(stdout);
				break;
			}
		}
		SSL_shutdown(ssl);
	}
sock_exit:
	close(sock);
exit:
	if(ssl)
		SSL_free(ssl);
	if(ctx)
		SSL_CTX_free(ctx);
}
