#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>

#include <unistd.h>

#include <sys/syscall.h>

#define FS_GET_MAX_IDX		3
#define FS_GET_NAME_BY_IDX	2
#define FS_GET_IDX_BY_NAME	1

int dump_flag = 0;

void init(int argc, char *argv[])
{
	char opt  = 0;

	while((opt = getopt(argc, argv, "l")) != -1){
		switch(opt){
		case 'l':
			dump_flag = 1;
			break;
		default:
			break;
		}
	}
}

void main(int argc, char *argv[])
{
	char buf[64];
	int i = 0;
	int max = syscall(SYS_sysfs, FS_GET_MAX_IDX);
	
	for(; i <= max; i++){
		memset(buf, 0, sizeof(buf));
		syscall(SYS_sysfs, FS_GET_NAME_BY_IDX, i, buf); 
		printf("%02d	%s\n",  i, buf);
	}
}
