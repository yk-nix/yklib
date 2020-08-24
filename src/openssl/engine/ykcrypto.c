#include <openssl/engine.h>
#include <openssl/rand.h>

#define ENGINE_EXIT_SUCCESS	1
#define ENGINE_EXIT_ERROR	0

extern RAND_METHOD ykrand;

static const char *engine_id   = "ID_ykcrypto";
static const char *engine_name = "ykcrypto";

static int bind_helper(ENGINE *e) {
	int ret;
	
	ret = ENGINE_set_id(e, engine_id);
	if(ret != ENGINE_EXIT_SUCCESS)
		goto exit;
	ret = ENGINE_set_name(e, engine_name);
	if(ret != ENGINE_EXIT_SUCCESS)
		goto exit;
	ret = ENGINE_set_RAND(e, &ykrand);
	if(ret != ENGINE_EXIT_SUCCESS)
		goto exit;
exit:	
	return ret;
}

void ENGINE_load_ykcrypto(void) {
	ENGINE *e = ENGINE_new();
	if(!e)
		return;
	if(bind_helper(e) != ENGINE_EXIT_SUCCESS)
		goto exit;
	ENGINE_add(e);
exit:
	ENGINE_free(e);
}
