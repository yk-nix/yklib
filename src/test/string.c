#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>

int base_dir_test(int argc, char *argv[]) {
	if(argc < 2)
		return 1;
	char *path = strdup(argv[1]);
	char *dir  = dirname(path);
	printf("%s\n", dir);
	free(path);
	path = strdup(argv[1]);
	char *base = basename(path);
	printf("%s\n", base);
	free(path);
}

int strtok_test(int argc, char *argv[]) {
	if(argc < 2)
		return 1;
	char *t;
	char *s = strdup(argv[1]);
	int count = 0;
	printf("%p  %s\n", s, s);
	while((t = strtok(s, ":"))) {
		printf("%d  %p  %s\n", ++count, t, t);
		s = NULL;
	}
	free(s);
	return 0;
}

int strstr_test(int argc, char *argv[]) {
	if(argc < 3)
		return 1;
	char *s = strdup(argv[1]);
	printf("%p  %s\n", s, s);
	char *p = strstr(s, argv[2]);
	if(p)
		printf("%p  %s\n", p, p);
	else
		printf("not found\n");
	return 0;
}

int strspn_test(int argc, char *argv[]) {
	if(argc < 2)
		return 1;
	int len = strspn(argv[1], "0123456789");
	printf("%.*s\n", len, argv[1]);
	return 0;
}

int strpbrk_test(int argc, char *argv[]) {
	if(argc < 2)
		return 1;
	char *p = strpbrk(argv[1], "0123456789");
	printf("%p   %s\n", argv[1], argv[1]);
	if(p)
		printf("%p  %s\n", p, p);
	else
		printf("not found\n");
}

int main(int argc, char *argv[]) {
	return strpbrk_test(argc, argv);
	//return strspn_test(argc, argv);
	//return strstr_test(argc, argv);
	//return strtok_test(argc, argv);
}
