#include <stdio.h>
#include <string.h>
#include <openssl/engine.h>

#include "ykcrypto.h"

void main(int argc, char *argv[]) {
	ENGINE_load_builtin_engines();
	//ENGINE_load_ykcrypto();
	ENGINE *e = ENGINE_get_first();
	int count = 1;
	while(e) {
		printf("%02d  id: %s   name:%s\n", count, ENGINE_get_id(e), ENGINE_get_name(e));
		e = ENGINE_get_next(e);
		count++;
	}
}
