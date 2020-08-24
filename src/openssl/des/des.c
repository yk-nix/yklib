#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <openssl/des.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>

#define DES_BLOCK_SIZE 	8

extern yk_hexdump(char *, int);
static char *progname = "des-test";
static const EVP_CIPHER *des_cipher;

void dump_cblock(DES_cblock *d) {
	int i = 0;
	for(i = 0; i < 8; i++) {
		printf("%02x ", (*d)[i]);
	}
	printf("      | ");
	for(i = 0; i < 8; i++) {
		printf("%c", isprint((*d)[i]) ? (*d)[i] : '.');
	}
	printf("\n");
}

int test_old(int argc, char *argv[]) {
	DES_cblock ptxt;
	DES_cblock ctxt;
	
	/* 01) generate random key */
	DES_cblock key;
	if(DES_random_key(&key) == 0){
		printf("error: DES_random_key: %s\n", strerror(errno));
	}
	dump_cblock(&key);
	/* convert into a DES_key_schedule */
	DES_key_schedule kschd;
	if(DES_set_key_checked(&key, &kschd)) {
		printf("error: DES_set_key_checked: %s\n", strerror(errno));
	}
	/* set key to odd */
	DES_set_odd_parity(&key);
	dump_cblock(&key);

	/* check if the key is weak or not */
	if(DES_is_weak_key(&key)) {
		printf("the is weak.\n");
	}
	else {
		printf("the is strong enough\n");
	}
	printf("\n");

	/* encrypt with DES-ECB(Electronic Code Book) */
	memset(ptxt, 0, sizeof(ptxt));
	memset(ctxt, 0, sizeof(ctxt));
	sprintf(ptxt, "yokawyl");
	DES_ecb_encrypt(&ptxt, &ctxt, &kschd, DES_ENCRYPT);
	dump_cblock(&ctxt);
	memset(ptxt, 0, sizeof(ptxt));
	DES_ecb_encrypt(&ctxt, &ptxt, &kschd, DES_DECRYPT);
	dump_cblock(&ptxt);
	printf("\n");

	/* encrypt/decrypt with DES-ECB3 */
	DES_cblock k1, k2, k3;
	DES_key_schedule ks1, ks2, ks3;
	DES_random_key(&k1);
	DES_random_key(&k2);
	DES_random_key(&k3);
	DES_set_key_checked(&k1, &ks1);
	DES_set_key_checked(&k2, &ks2);
	DES_set_key_checked(&k3, &ks3);
	memset(ptxt, 0, sizeof(ptxt));
	memset(ctxt, 0, sizeof(ctxt));
	sprintf(ptxt, "yokawyl");
	DES_ecb3_encrypt(&ptxt, &ctxt, &ks1, &ks2, &ks3, DES_ENCRYPT);
	dump_cblock(&ctxt);
	memset(ptxt, 0, sizeof(ptxt));
	DES_ecb3_encrypt(&ctxt, &ptxt, &ks1, &ks2, &ks3, DES_DECRYPT);
	dump_cblock(&ptxt);
	printf("\n");
}

void cache_hexdump(BIO *cache) {
	int size = BIO_ctrl_pending(cache);
	if(size <= 0)
		return;
	char *data = NULL;
	BIO_get_mem_data(cache, &data);
	if(data)
		yk_hexdump(data, size);
}

/* orignal API for DES algorithm */
void test1(int argc, char *argv[]) {
	
}


/* EVP API from DES algorithm */
void test2(int argc, char *argv[]) {
	char key[EVP_MAX_KEY_LENGTH] = "hello,world";
	char iv[EVP_MAX_IV_LENGTH] = "1234567890123456";
	char cbuf[1024], pbuf[1024];
	int clen = sizeof(cbuf);
	int plen = sizeof(pbuf);
	BIO *cache1 = BIO_new(BIO_s_mem());
	BIO *cache2 = BIO_new(BIO_s_mem());
	ERR_load_crypto_strings();
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	EVP_CIPHER_CTX_init(ctx);
	if(EVP_CipherInit_ex(ctx, des_cipher, NULL, key, iv, 1) <= 0) {
		ERR_print_errors_fp(stderr);
		goto exit1;
	}
	if(argc > 1) {
		char *txt = argv[1];
		int len = strlen(txt);
		int left = len;
		int idx = 0;
		while(left > 0) {
			memset(cbuf, 0, sizeof(cbuf));
			if(left > DES_BLOCK_SIZE) {
				if(EVP_CipherUpdate(ctx, cbuf, &clen, txt + idx, DES_BLOCK_SIZE) <= 0) {
					ERR_print_errors_fp(stderr);
					goto exit2;
				}
				BIO_write(cache1, cbuf, clen);
				idx  += DES_BLOCK_SIZE;
				left -= DES_BLOCK_SIZE;
			}
			else {
				if(EVP_CipherUpdate(ctx, cbuf, &clen, txt + idx, left) <= 0) {
					ERR_print_errors_fp(stderr);
					goto exit2;
				}
				BIO_write(cache1, cbuf, clen);
				idx  += left;
				left -= left;
			}
		}
		memset(cbuf, 0, sizeof(cbuf));
		if(EVP_CipherFinal_ex(ctx, cbuf, &clen) <= 0) {
			ERR_print_errors_fp(stderr);
			goto exit2;
		}
		BIO_write(cache1, cbuf, clen);
		cache_hexdump(cache1);
		
		EVP_CIPHER_CTX_cleanup(ctx);
		memset(pbuf, 0, sizeof(pbuf));
		len = BIO_ctrl_pending(cache1);
		left = len;
		idx = 0;
		char *data = NULL;
		BIO_get_mem_data(cache1, &data);
		if(!data) {
			ERR_print_errors_fp(stderr);
			goto exit1;
		}
		if(EVP_DecryptInit_ex(ctx, des_cipher, NULL, key, iv) <= 0) {
			ERR_print_errors_fp(stderr);
			goto exit1;
		}
		while(left > 0) {
			memset(pbuf, 0, sizeof(pbuf));
			if(left > DES_BLOCK_SIZE) {
				if(EVP_DecryptUpdate(ctx, pbuf, &plen, data + idx, DES_BLOCK_SIZE)  <= 0) {
					ERR_print_errors_fp(stderr);
					goto exit2;
				} 
				BIO_write(cache2, pbuf, plen);
				idx  += DES_BLOCK_SIZE;
				left -= DES_BLOCK_SIZE;
			}
			else {
				if(EVP_DecryptUpdate(ctx, pbuf, &plen, data + idx, left)  <= 0) {
					ERR_print_errors_fp(stderr);
					goto exit2;
				} 
				BIO_write(cache2, pbuf, plen);
				idx  += left;
				left -= left;
			}
		}
		memset(pbuf, 0, sizeof(pbuf));
		if(EVP_DecryptFinal_ex(ctx, pbuf, &plen) <= 0) {
			ERR_print_errors_fp(stderr);
			goto exit2;
		}
		BIO_write(cache2, pbuf, plen);
		cache_hexdump(cache2);
	}
exit2:
	EVP_CIPHER_CTX_cleanup(ctx);
exit1:
	BIO_free(cache1);
	BIO_free(cache2);
	EVP_CIPHER_CTX_free(ctx);
}

void init(int argc, char *argv[]) {
	des_cipher = EVP_des_ede3_cfb();
	printf("%s\n", EVP_CIPHER_name(des_cipher));
}

void main(int argc, char *argv[]) {
	init(argc, argv);
	test2(argc, argv);
}

