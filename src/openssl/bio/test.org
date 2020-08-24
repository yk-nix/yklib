#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <openssl/bio.h>

void main(int argc, char *argv[]) {
	int bytes;
	char buf[1024];
	char cache[1024];
	
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "hello, world!");
	//BIO *bio = BIO_new(BIO_s_mem());
	BIO *bio = BIO_new_mem_buf(buf, -1);
	//BIO_puts(bio, buf);
	char *data;
	int len = BIO_get_mem_data(bio, &data);
	
	if(BIO_eof(bio)) {
		printf("there is no data.\n");
	}
	else {
		while(BIO_ctrl_pending(bio)) {
			printf("data size: %ld\n", BIO_ctrl_pending(bio));
			memset(cache, 0, sizeof(cache));
			bytes = BIO_read(bio, cache, sizeof(cache));
			if(bytes > 0)
				printf("%s\n", cache);
			else
				printf("error: BIO_gets\n");
		}
	}
	BIO_free_all(bio);
	printf("%*s\n", len, data);
}
