
#include <stdio.h>
#include <openssl/engine.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>

#define YK_SET_RSA_PRIVATE_KEY  1

static int yk_get_random_bytes(unsigned char *buf, int num) {
	int i = 0;
	for(i = 0; i <num; i++)
		buf[i] = i;
	return 1;
}

static int generate_rsa_key(RSA *rsa, int bits, BIGNUM *e, BN_GENCB *cb) {
	printf("generate_rsak_key\n");
	return 1;
}

static int rsa_pub_enc(int flen, const unsigned char *from, unsigned char *to, RSA *rsa, int padding) {
	printf("rsa_pub_enc\n");
	return 1;
}
static int rsa_pub_dec(int flen, const unsigned char *from, unsigned char *to, RSA *rsa, int padding) {
	printf("rsa_pub_dec\n");
	return 1;
}
static int rsa_priv_enc(int flen, const unsigned char *from, unsigned char *to, RSA *rsa, int padding) {
	printf("rsa_priv_enc\n");
	return 1;
}
static int rsa_priv_dec(int flen, const unsigned char *from, unsigned char *to, RSA *rsa, int padding) {
	pritnf("rsa_priv_dec\n");
	return 1;
}

static RSA_METHOD yk_rsa = {
	"yk rsa",
	rsa_pub_enc,
	rsa_pub_dec,
	rsa_priv_enc,
	rsa_priv_dec,
	NULL,
	NULL,
	NULL,
	NULL,
	RSA_FLAG_SIGN_VER,
	NULL,
	NULL,
	NULL,
	generate_rsa_key
};

static RAND_METHOD yk_rand = {
	NULL,
	yk_get_random_bytes,
	NULL,
	NULL,
	NULL,
	NULL
};

static const char *engine_yk_id   = "ID_yk";
static const char *engine_yk_name = "yktest";

static int yk_init(ENGINE *e) {
	printf("yk_init\n");
	return 1;
}

static int yk_destroy(ENGINE *e) {
	printf("yk_destory\n");
	return 1;
}

static int yk_finish(ENGINE *e) {
	printf("yk_finshi\n");
	return 1;
}

static EVP_PKEY *yk_load_privkey(ENGINE *e, const char *key_id, UI_METHOD *ui_method, void *cb_data) {
	printf("yk_load_privkey\n");
}

static int yk_ctrl(ENGINE *e, int cmd, long i, void *p, void (*f)(void)) {
	switch(cmd) {
	case YK_SET_RSA_PRIVATE_KEY:
		yk_load_privkey(e, p, NULL, NULL);
		break;
	default:
		printf("yk_ctl: error\n");
		break;
	}
	return 0;
}

static EVP_PKEY *yk_load_pubkey(ENGINE *e, const char *key_id, UI_METHOD *ui_method, void *cb_data) {
	printf("yk_load_pubkey\n");
	return NULL;
}

static const ENGINE_CMD_DEFN yk_cmd_defns[] = {
	{ENGINE_CMD_BASE, "SO_PATH", "Specifies the path to 'yk' shared library", ENGINE_CMD_FLAG_STRING},
	{0, NULL, NULL, 0}
};

static int yk_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key, const unsigned char *iv, int enc) {
	printf("yk_init_key\n");
	return 1;
}

static int yk_cipher_enc(EVP_CIPHER_CTX *ctx, unsigned char *out, const unsigned char *in, unsigned int inl) {
	printf("yk_cipher_enc\n");
	return 1;
}

static const EVP_CIPHER EVP_yk_c = {
	NID_des_ecb,
	1,
	8,
	0,
	8,
	yk_init_key,
	yk_cipher_enc,
	NULL,
	1,
	NULL,
	NULL,
	NULL,
	NULL
};

const EVP_CIPHER *EVP_yk_cipher(void) {
	return (&EVP_yk_c);
}

static int cipher_nids[] = {
	NID_des_ecb,
	NID_des_ede3_cbc,
	0
};


static int yk_cihpers(ENGINE *e, const EVP_CIPHER **cipher, const int **nids, int nid) {
	if(cipher == NULL) {
		*nids = cipher_nids;
		return (sizeof(cipher_nids)-1)/sizeof(cipher_nids[0]);
	switch(nid) {
	case NID_des_ecb:
		*cipher = EVP_yk_cipher();
		break;
	default: 
		return 0;
	}
	return 1;
}

static int init(EVP_MD_CTX *ctx) {
	printf("md init\n");
	return 1;
}

static int update(EVP_MD_CTX *ctx, const void *data, size_t count) {
	printf("md update\n");
	return 1;
}

static int final(EVP_MD_CTX *ctx, unsigned char *md) {
	printf("md final\n");
	return 1;
}

int yk_sign(int type, const unsigned char *m, unsigned int m_length, unsigned char *sigret, 
		unsigned int siglen, void *key) {
	printf("ykSgin\n");
	return 1;
}

int yk_verify(int type, const unsigned char *m, unsigned int m_length, const char *sigbuf,
		unsigned int signlen, void *key) {
	printf("ykVerify\n");
	return 1;
}

static int digest_nids[] = {
	NID_sha1,
	NID_md5,
	0
};

static const EVP_MD ykmd = {
	NID_sha1,
	NID_sha1WithRSAEncryption,
	SHA_DIGEST_LENGTH,
	0,
	init,
	update,
	final,
	NULL,
	NULL,
	yk_sign,
	yk_verify,
	6
};

static EVP_MD *EVP_yk_md() {
	return (&ykmd);
}

static int yk_md(ENGINE *e, const EVP_MD **digest, const int **nids, int nid) {
	if(digest == NULL) {
		*nids = digest_nids;
		return (sizeof(digest_nids)-1)/sizeof(digest_nids[0]);
	}
	switch(nid) {
	case NID_sha1:
		*digest = EVP_yk_md();
		break;
	default:
		return 0;
	}
	return 1;
}

static int bind_helper(ENGINE *e) {
	int ret;
	
	ret = ENGINE_set_id(e, engine_yk_id);
	if(ret != 1) {
		printf("ENGINE_set_id failed\n");
		return 0;
	}

	ret = ENGINE_set_name(e, engine_yk_name);
	if(ret != 1) {
		printf("ENGINE_set_name failed\n");
		return 0;
	}
	
	ret = ENGINE_set_RSA(e, &yk_rsa);
	if(ret != 1) {
		printf("ENGINE_set_RSA failed\n");
		return 0;
	}
	
	ret = ENGINE_set_RAND(e, &yk_rand);
	if(ret != 1) {
		printf("ENGINE_set_RAND failed\n");
		return 0;
	}

	ret = ENGINE_set_destory_funciton(e, &yk_destroy);
	if(ret != 1) {
		printf("ENGINE_set_destroy_function failed\n");
		return 0;
	}
	
	ret = ENGINE_set_init_function(e, &yk_init);
	if(ret != 1) {
		printf("ENGINE_set_init_function failed\n");
		return 0;
	}

	ret = ENGINE_set_finish_function(e, &yk_finish);
	if(ret != 1) {
		printf("ENGINE_set_finish_function failed\n");
		return 0;
	}
	
	ret = ENGINE_set_ctrl_function(e, &yk_ctrl);
	if(ret != 1) {
		printf("ENGINE_set_ctrl_function failed\n");
		return 0;
	}

	ret = ENGINE_set_ctrl_function(e, &yk_ctrl);
	if(ret != 1) {
		printf("ENGINE_set_ctrl_function failed\n");
		return 0;
	}
	
	ret = ENGINE_set_load_privkey_function(e, &yk_load_privkey);
	if(ret != 1) {
		printf("ENGINE_set_load_privkey_function failed\n");
		return 0;
	}
	
	ret = ENGINE_set_load_pubkey_function(e, &yk_load_pubkey);
	if(ret != 1) {
		printf("ENGINE_set_load_pubkey_function failed\n");
		return 0;
	}
	
	ret = ENGINE_set_cmd_defns(e, &yk_cmd_defns);
	if(ret != 1) {
		printf("ENGINE_set_cmd_defns failed\n");
		return 0;
	}
	
	ret = ENGINE_set_cmd_ciphers(e, &yk_ciphers);
	if(ret != 1) {
		printf("ENGINE_set_ciphers failed\n");
		return 0;
	}
	
	ret = ENGINE_set_digest(e, &yk_md);
	if(ret != 1) {
		printf("ENGINE_set_digest failed\n");
		return 0;
	}
	
	return 1;
}

static ENGINE *engine_yk_cipher(void) {
	ENGINE *ret = ENGINE_new();
	if(!ret)
		return NULL;
	if(!bind_helper(ret)) {
		ENGINE_free(ret):
		return NULL;
	}
	return ret;
}

void ENGINE_load_yk_cipher(void) {
	ENGINE *e = engine_yk_cipher();
	if(!e)
		return;
	ENGINE_add(e);
	ENGINE_free(e);
	ERR_clear_error();
}


