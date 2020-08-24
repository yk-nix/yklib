#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/acl.h>

void main(int argc, char *argv[]){
	acl_t   acl = NULL;
	char *acltxt;
	int ret;
	acl_entry_t acl_entry;
	acl_permset_t acl_permset;
	uid_t admin = 1007;
	
	if(argc < 2) return;
	
	acl = acl_get_file(argv[1], ACL_TYPE_ACCESS);
	if(acl == NULL){
		printf("error: acl_get_file: %s\n", strerror(errno));
		goto exit;
	}
	/*
	ret = acl_create_entry(&acl, &acl_entry);
	if(ret) {
		printf("error: acl_create_entry: %s\n", strerror(errno));
	}
	else {
		acl_get_permset(acl_entry, &acl_permset);
		acl_add_perm(acl_permset, ACL_READ);
		acl_add_perm(acl_permset, ACL_WRITE);
		acl_add_perm(acl_permset, ACL_EXECUTE);
		acl_set_tag_type(acl_entry, ACL_MASK);
		acl_set_permset(acl_entry, acl_permset);
	}
	*/
	ret = acl_create_entry(&acl, &acl_entry);
	if(ret) {
		printf("error: acl_create_entry: %s\n", strerror(errno));
	}
	else {
		acl_get_permset(acl_entry, &acl_permset);
		acl_add_perm(acl_permset, ACL_READ);
		acl_add_perm(acl_permset, ACL_WRITE);
		acl_add_perm(acl_permset, ACL_EXECUTE);
		acl_set_tag_type(acl_entry, ACL_USER);
		acl_set_qualifier(acl_entry, &admin);
		acl_set_permset(acl_entry, acl_permset);
	}
	acltxt = acl_to_text(acl, NULL);
	if(acltxt == NULL){
		printf("error: acl_to_txt: %s\n", strerror(errno));
		goto exit;
	}
	printf("%s", acltxt);
	printf("size of acl: %d\n", acl_size(acl));
	printf("acl valid: %s\n", acl_valid(acl) ? "NG":"OK");
exit:
	if(acl != NULL)
		acl_free(acl);
	if(acltxt != NULL)
		acl_free(acltxt);
}
