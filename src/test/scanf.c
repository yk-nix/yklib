#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void main(int argc, char *argv[])
{
	int err;
	int age = 0, height = 0;
	char name[5];
	memset(name, 0, sizeof(name));
	
	printf("%ld\n", 2^3);
	err = scanf("%s %d %d", name, &age, &height);
	if(err <= 0){
		printf("error: scanf: %s\n", strerror(errno));
		return;
	} 
	printf("analyze %d characters\n", err);

	printf("name: %s      age:%d      height: %d\n", name, age, height);
}
