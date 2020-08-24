#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>

#include <openssl/bio.h>
#include <openssl/evp.h>

void b64_encode() {
	int ret;
	char cache[1024];
	BIO *out, *b64;
	
	out = BIO_new_fp(stdout, BIO_NOCLOSE);
	b64 = BIO_new(BIO_f_base64());
	BIO_push(b64, out);
	while(1) {
		memset(cache, 0, sizeof(cache));
		scanf("%s", cache);
		if(strcasecmp(cache, "exit") == 0)
			break;
		BIO_write(b64, cache, strlen(cache));
		BIO_flush(b64);
	}
	BIO_free_all(b64);
}

void b64_decode() {
	int ret;
	char cache[1024];
	BIO *in, *b64, *out;
	
	in  = BIO_new_fp(stdin, BIO_NOCLOSE);
	out = BIO_new_fp(stdout, BIO_NOCLOSE);
	b64 = BIO_new(BIO_f_base64());
	BIO_push(b64, in);
	memset(cache, 0, sizeof(cache));
	while((ret = BIO_read(b64, cache, sizeof(cache))) > 0) {
		BIO_write(out, cache, ret);
	}
	BIO_flush(out);
	BIO_free_all(b64);
}

void b64_encode_file(const char *file) {
	int bytes = 0;
	char cache[1024];
	BIO *in  = BIO_new_file(file, "r");
	BIO *out = BIO_new_fp(stdout, BIO_NOCLOSE);
	BIO *b64 = BIO_new(BIO_f_base64());
	BIO_push(b64, out);
	memset(cache, 0, sizeof(cache));
	while((bytes = BIO_read(in, cache, sizeof(cache))) > 0)
		BIO_write(b64, cache, bytes);
	BIO_flush(b64);
	BIO_free(in);
	BIO_free_all(b64);
}

void b64_decode_file(const char *file) {
	int bytes = 0;
	char cache[1024];
	BIO *in  = BIO_new_file(file, "r");
	BIO *out = BIO_new_fp(stdout, BIO_NOCLOSE);
	BIO *b64 = BIO_new(BIO_f_base64());
	BIO_push(b64, in);
	memset(cache, 0, sizeof(cache));
	while((bytes = BIO_read(b64, cache, sizeof(cache))) > 0)
		BIO_write(out, cache, bytes);
	BIO_flush(out);
	BIO_free(out);
	BIO_free_all(b64);
}

#define CIPHER_ENCODE 1
#define CIPHER_DECODE 0

void des_encode_file(const char *in_file, const char *out_file, const EVP_CIPHER *cipher,
		 const char *key, const char *iv) {
	if(in_file == NULL || out_file == NULL)
		return;
	int bytes = 0;
	char cache[1024];
	BIO *in  = BIO_new_file(in_file, "r");
	BIO *out = BIO_new_file(out_file, "w");
	BIO *c = BIO_new(BIO_f_cipher());
	BIO_set_cipher(c, cipher, key, iv, CIPHER_ENCODE);
	BIO_push(c, out);
	while((bytes = BIO_read(in, cache, sizeof(cache))) > 0)
		BIO_write(c, cache, bytes);
	BIO_flush(c);
	BIO_free(in);
	BIO_free_all(c);
}
void des_decode_file(const char *in_file, const char *out_file, const EVP_CIPHER *cipher,
		 const char *key, const char *iv) {
	if(in_file == NULL || out_file == NULL)
		return;
	int bytes = 0;
	char cache[1024];
	BIO *in  = BIO_new_file(in_file, "r");
	BIO *out = BIO_new_file(out_file, "w");
	BIO *c = BIO_new(BIO_f_cipher());
	BIO_set_cipher(c, cipher, key, iv, CIPHER_DECODE);
	BIO_push(c, in);
	while((bytes = BIO_read(c, cache, sizeof(cache))) > 0)
		BIO_write(out, cache, bytes);
	BIO_flush(out);
	BIO_free(out);
	BIO_free_all(c);
}
void des_decode(const char *in_file, const EVP_CIPHER *cipher,
		 const char *key, const char *iv) {
	if(in_file == NULL)
		return;
	int bytes = 0;
	char cache[1024];
	BIO *in  = BIO_new_file(in_file, "r");
	BIO *out = BIO_new_fp(stdout, BIO_NOCLOSE);
	BIO *c = BIO_new(BIO_f_cipher());
	BIO_set_cipher(c, cipher, key, iv, CIPHER_DECODE);
	BIO_push(c, in);
	while((bytes = BIO_read(c, cache, sizeof(cache))) > 0)
		BIO_write(out, cache, bytes);
	BIO_flush(out);
	printf("status: %d\n", BIO_get_cipher_status(c));
	BIO_free(out);
	BIO_free_all(c);
}

int operation = CIPHER_ENCODE;
char *ifile, *ofile;
void init(int argc, char *argv[]) {
	char opt;
	struct option opts[] = {
		{ "decode", no_argument, NULL, 'd' },
		{ "help", no_argument, NULL, 'h' },
	};
	int idx = 0;
	while((opt = getopt_long(argc, argv, "dh", opts, &idx)) != -1) {
		switch(opt) {
		case 'd':
			operation = CIPHER_DECODE;
			break;
		default:
			break;
		}
	}
	ifile = argv[optind];
	if(optind + 1 < argc)
		ofile = argv[optind + 1];
}

char *ptxt = "hi";
extern void yk_hexdump(char *, int);
void test() {
	char cache[1024];
	char key[EVP_MAX_KEY_LENGTH] = "hello,world";
	char iv[EVP_MAX_IV_LENGTH] = "coolyoka";
	BIO *des_enc = BIO_new(BIO_f_cipher());
	BIO *b64_enc = BIO_new(BIO_f_base64());
	BIO *source  = BIO_new(BIO_s_bio());
	BIO *sink    = BIO_new(BIO_s_bio());
	BIO *b64_dec = BIO_new(BIO_f_base64());
	BIO *des_dec = BIO_new(BIO_f_cipher());
	BIO_set_cipher(des_enc, EVP_des_ede3_ecb(), key, iv, 1);
	BIO_make_bio_pair(source, sink);
	BIO_set_cipher(des_dec, EVP_des_ede3_ecb(), key, iv, 0);
	BIO_push(b64_enc, source);
	BIO_push(des_enc, b64_enc);
	BIO_push(b64_dec, sink);
	BIO_push(des_dec, b64_dec);
	BIO_write(des_enc, ptxt, strlen(ptxt));
	BIO_flush(des_enc);
	memset(cache, 0, sizeof(cache));
	int bytes = BIO_read(sink, cache, sizeof(cache));
	yk_hexdump(cache, bytes);
	BIO_free_all(des_enc);
	BIO_free_all(des_dec);
}

void test1(void) {
	char cache[1024];
	char key[EVP_MAX_KEY_LENGTH] = "hello,world";
	char iv[EVP_MAX_IV_LENGTH] = "coolyoka";
	BIO *des_enc = BIO_new(BIO_f_cipher());
	BIO *b64_enc = BIO_new(BIO_f_base64());
	BIO *source  = BIO_new(BIO_s_bio());
	BIO *sink    = BIO_new(BIO_s_bio());
	BIO *b64_dec = BIO_new(BIO_f_base64());
	BIO *des_dec = BIO_new(BIO_f_cipher());
	BIO_set_cipher(des_enc, EVP_des_ede3_ecb(), key, iv, 1);
	BIO_make_bio_pair(source, sink);
	BIO_set_cipher(des_dec, EVP_des_ede3_ecb(), key, iv, 0);
	BIO_push(des_enc, b64_enc);
	BIO_push(b64_enc, source);
	BIO_push(b64_dec, sink);
	BIO_push(des_dec, b64_dec);
	BIO_write(des_enc, ptxt, strlen(ptxt));
	BIO_flush(des_enc);
	memset(cache, 0, sizeof(cache));
	int bytes = BIO_read(b64_dec, cache, sizeof(cache));
	yk_hexdump(cache, bytes);
	BIO_free_all(des_enc);
	BIO_free_all(des_dec);
}

void test2(void) {
	char cache[1024];
	char key[EVP_MAX_KEY_LENGTH] = "hello,world";
	char iv[EVP_MAX_IV_LENGTH] = "coolyoka";
	BIO *des = BIO_new(BIO_f_cipher());
	BIO *out = BIO_new_fp(stdout, BIO_NOCLOSE);
	BIO *b64 = BIO_new(BIO_f_base64());
	BIO_set_cipher(des, EVP_des_ede3_ecb(), key, iv, 1);
	BIO_push(des, b64);
	BIO_push(b64, out);
	BIO_write(des, ptxt, strlen(ptxt));
	BIO_flush(des);
	BIO_free_all(des);
}

void test3(void) {
	char cache[1024];
	char key[EVP_MAX_KEY_LENGTH] = "hello,world";
	char iv[EVP_MAX_IV_LENGTH] = "coolyoka";
	BIO *des_enc = BIO_new(BIO_f_cipher());
	BIO *b64_enc = BIO_new(BIO_f_base64());
	BIO *source  = BIO_new(BIO_s_bio());
	BIO *sink    = BIO_new(BIO_s_bio());
	BIO *b64_dec = BIO_new(BIO_f_base64());
	BIO *des_dec = BIO_new(BIO_f_cipher());
	BIO_set_cipher(des_enc, EVP_des_ede3_ecb(), key, iv, 1);
	BIO_make_bio_pair(source, sink);
	BIO_set_cipher(des_dec, EVP_des_ede3_ecb(), key, iv, 0);
	BIO_push(des_enc, b64_enc);
	BIO_push(b64_enc, source);
	BIO_write(des_enc, ptxt, strlen(ptxt));
	BIO_flush(des_enc);
	memset(cache, 0, sizeof(cache));
	int bytes = BIO_read(des_dec, cache, sizeof(cache));
	yk_hexdump(cache, bytes);
	ERR_print_errors_fp(stdout);
	printf("cipher status: %d\n", BIO_get_cipher_status(des_dec));
	BIO_free_all(des_enc);
	BIO_free_all(des_dec);
}

void test4(void) {
	BIO *cache  = BIO_new(BIO_s_mem());
	BIO *b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(cache, BIO_FLAGS_BASE64_NO_NL);
	BIO_push(b64, cache);
	
	char *ctxt = "RG9uJ3QgdG91Y2ggbXkgcHVyaW4hIExldCBpdCBhbG9uZSEh\n";
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	//BIO_write(b64, ptxt, strlen(ptxt));
	//BIO_flush(b64);
	BIO_write(cache, ctxt, strlen(ctxt));
	BIO_flush(cache);
	int bytes = BIO_read(b64, buf, sizeof(buf));
	BIO_free_all(b64);
	
	printf("%s\n", buf);
}

void test5(void) {
	char buf[1024];
	BIO *cache = BIO_new(BIO_s_mem());
	BIO *b64   = BIO_new(BIO_f_base64());
	BIO *out   = BIO_new_fp(stdout, BIO_NOCLOSE);
	BIO_push(b64, cache);
	while(1) {
		memset(buf, 0, sizeof(buf));
		scanf("%s", buf);
		if(strcasecmp(buf, "exiT") == 0)
			break;
		strcat(buf, "\n");
		BIO_write(cache, buf, strlen(buf));
		printf("cache: %d\n", BIO_ctrl_pending(cache));
		memset(buf, 0, sizeof(buf));
		int bytes = BIO_read(b64, buf, sizeof(buf));
		printf("bytes: %d\n", bytes);
		BIO_write(out, buf, bytes);
		BIO_flush(out);
	}
	BIO_free_all(cache);
}

void main(int argc, char *argv[]) {
	/*
	if(argc == 2)
		ptxt = argv[1];
	printf("test: \n");
	test();
	printf("\ntest1:\n");
	test1();
	printf("\ntest2:\n");
	test2();
	printf("\ntest3:\n");
	test3();
	*/
	test5();
}
