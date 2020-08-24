#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <openssl/err.h>
#include <openssl/bn.h>

int calc_private_key(BIGNUM *pubkey, BIGNUM *euila, BIGNUM *prvkey, BN_CTX *ctx) {
	BIGNUM *n1, *n2, *n3, *count, *one;
	n1 = BN_CTX_get(ctx);
	n2 = BN_CTX_get(ctx);
	n3 = BN_CTX_get(ctx);
	one = BN_CTX_get(ctx);
	count = BN_CTX_get(ctx);
	BN_dec2bn(&one, "1");
	BN_dec2bn(&count, "0");
	count = BN_CTX_get(ctx);
	while(1) {
		BN_add(count, count, one);
		BN_mul(n1, euila, count, ctx);
		BN_add(n2, n1, one);
		BN_div(prvkey, n3, n2, pubkey, ctx);
		if(BN_is_zero(n3))
			return 0;
	}
	return -1;
}

int crypt(char *m, char *c, BIGNUM *key, BIGNUM *modulo, int size, BN_CTX *ctx) {
	BIGNUM *mtxt, *ctxt;
	mtxt = BN_CTX_get(ctx);
	ctxt = BN_CTX_get(ctx);
	BN_bin2bn(m, 64, mtxt);
	printf("mtxt:%s\n", BN_bn2hex(mtxt));
	BN_mod_exp(ctxt, mtxt, key, modulo, ctx);
	BN_bn2bin(ctxt, c);
	printf("ctxt:%s\n", BN_bn2hex(ctxt));
	return 0;
}

int main(int argc, char argv[]) {
	char mbuf[64], cbuf[64];
	BIGNUM *p, *q, *n, *e, *d, *euila, *p1, *q1;
	BIGNUM *one;
	BN_CTX *ctx = BN_CTX_new();
	if(ctx == NULL)
		return -1;
	BN_CTX_start(ctx);
	p = BN_CTX_get(ctx);
	p1 = BN_CTX_get(ctx);
	q = BN_CTX_get(ctx);
	q1 = BN_CTX_get(ctx);
	n = BN_CTX_get(ctx);
	e = BN_CTX_get(ctx);
	d = BN_CTX_get(ctx);
	one = BN_CTX_get(ctx);
	euila = BN_CTX_get(ctx);
	if(p == NULL || q == NULL || n == NULL || p1 == NULL || q1 == NULL ||
	   e == NULL || d == NULL || one == NULL || euila == NULL)
		goto exit;
	BN_dec2bn(&one, "1");
	BN_hex2bn(&e, "10001");
	BN_hex2bn(&p, "00d68812552d321fb8e840369e01f0ffa61bde3da0cfd05bada7a3a3ea749e9173");
	BN_hex2bn(&q, "00cc6b130f3c870b79448a71a151a49d82375b531dd56827c8ca3de4cbb5718a9b");
	BN_sub(p1, p, one);
	BN_sub(q1, q, one);
	BN_mul(euila, p1, q1, ctx);
	BN_mul(n, p, q, ctx);
	calc_private_key(e, euila, d, ctx);
	
	printf("p: %s\n  prime:%s\n", BN_bn2hex(p),
		BN_is_prime_ex(p, BN_prime_checks, ctx, NULL) ? "yes" : "no");
	printf("q: %s\n  prime:%s\n", BN_bn2hex(q),
		BN_is_prime_ex(q, BN_prime_checks, ctx, NULL) ? "yes" : "no");
	printf("e: %s\n  prime:%s\n", BN_bn2hex(e), 
		BN_is_prime_ex(e, BN_prime_checks, ctx, NULL) ? "yes" : "no");
	printf("n: %s\n  prime:%s\n", BN_bn2hex(n), 
		BN_is_prime_ex(n, BN_prime_checks, ctx, NULL) ? "yes" : "no");
	printf("d: %s\n  prime:%s\n", BN_bn2hex(d), 
		BN_is_prime_ex(d, BN_prime_checks, ctx, NULL) ? "yes" : "no");
	memset(mbuf, 0, sizeof(mbuf));
	memset(cbuf, 0, sizeof(cbuf));
	sprintf(mbuf, "hello,world");
	crypt(mbuf, cbuf,  e, n, 64, ctx);
	memset(mbuf, 0, sizeof(mbuf));
	crypt(cbuf, mbuf, d, n, 64, ctx);
	printf("%s\n", mbuf);
exit:
	if(ctx) {
		BN_CTX_end(ctx);
		BN_CTX_free(ctx);
	}
}
