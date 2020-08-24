#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/acl.h>

void acl_print(acl_t acl) {
	if(acl == NULL)
		return;
	printf("%s\n", acl_to_text(acl, NULL));
}

#ifdef _DEBUG_ACL_
void main(int argc, char *argv[]) {
	acl_t acl;
	
	if(argc < 2)
		return;
	acl = acl_get_file(argv[1], ACL_TYPE_ACCESS);
	if(acl == NULL) {
		printf("error: acl_get_file: %s: %s\n", argv[1], strerror(errno));
		goto exit;
	}
	acl_print(acl);
	if(acl_set_file(argv[1], ACL_TYPE_ACCESS, acl)) {
		printf("error: acl_set_file: %s: %s\n", argv[1], strerror(errno));
		goto exit;
	}
exit:
	if(acl)
		acl_free(acl);
}
#endif
