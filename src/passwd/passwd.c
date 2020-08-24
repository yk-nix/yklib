#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>

#include <pwd.h>
#include <shadow.h>

extern char *crypt(const char *key, const char *salt);

void dump_pwd(struct passwd *p) {
	if(p == NULL)
		return;
	printf("---------- %s --------\n", p->pw_name);
	printf("username: %s\n", p->pw_name);
	printf("password: %s\n", p->pw_passwd);
	printf(" user id: %d\n", p->pw_uid);
	printf("group id: %d\n", p->pw_gid);
	printf("usr info: %s\n", p->pw_gecos);
	printf("home dir: %s\n", p->pw_dir);
	printf(" sh prog: %s\n", p->pw_shell);
}

void dump_spwd(struct spwd *sp) {
	if(sp == NULL)
		return;
	printf("---------- %s --------\n", sp->sp_namp);
	printf("  login name: %s\n", sp->sp_namp);
	printf("    passwrod: %s\n", sp->sp_pwdp);
	printf(" last change: %d\n", sp->sp_lstchg);
	printf("  min change: %d\n", sp->sp_min);
	printf("  max change: %d\n", sp->sp_max);
	printf("   warn days: %d\n", sp->sp_warn);
	printf("elastic days: %d\n", sp->sp_inact);
	printf(" expire date: %d\n", sp->sp_expire); 
}

static int _verify_password(struct spwd *sp, const char *password) {
	char salt[24];
	int i = 0, j = 0;

	if(sp == NULL)
		return 0;
	
	memset(salt, 0, sizeof(salt));
	while(sp->sp_pwdp[i] != '\0'){
		salt[i] = sp->sp_pwdp[i];
		if(salt[i++] == '$') {
			j++;
			if(j == 3)
				break;
		}
	}
	return (!strcmp(sp->sp_pwdp, crypt(password, salt)));
}

int verify_password_by_name(const char *name, const char *password) {
	return _verify_password(getspnam(name), password);
}

int verify_password_by_uid(uid_t uid, const char *password) {
	struct passwd *p = getpwuid(uid);
	if(p)
		return _verify_password(getspnam(p->pw_name), password);
	return 0;
}

#ifdef _PWD_DEBUG_
void main(int argc, char *argv[]) {
	int i = 0;
	
	if(argc < 2)
		return;
	printf("name: %s   password: %s   verified result: %s\n",
		argv[1], argv[2], verify_password_by_name(argv[1], argv[2]) ? "OK" : "NG");
}
#endif
