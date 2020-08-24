#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <syscall.h>

struct person{
	char name[30];
	int  age;
	int  height;
};


void array_init_test(int argc, char *argv[])
{
	int i = 0;
	struct person person[10] = {
		[0 ... 6] = {
			.name = "none",
			.age  = -1,
			.height = -1,
		}
	};
	for(i = 0; i < 10; i++){
		printf("%02d  %s	%d	%d\n", i, person[i].name, person[i].age, person[i].height);
	}
}

#define FS_CMD_GETNUM		3
#define FS_CMD_GETNAMEBYIDX	2
#define FS_CMD_GETIDXBYNAME	1
void sysfs_test(int argc, char *argv[])
{
	int num = syscall(SYS_sysfs, FS_CMD_GETNUM);
	if(num < 0){
		printf("error: sysfs: %s\n", strerror(errno));	
		return;
	}
	//printf("fs num: %d\n", num);
	char fsname[128];
	int i = 0;
	for(; i < num; i++){
		memset(fsname, 0, sizeof(fsname));
		if(syscall(SYS_sysfs, FS_CMD_GETNAMEBYIDX, i, fsname)){
			printf("idx: %02d	error: %s\n", i, strerror(errno));
			continue;
		}
		printf("idx: %02d	fsname: %s\n", i, fsname);
	}
}

//#include <unistd.h>
#include <linux/sysctl.h>
void sysctl_test(int argc, char *argv[])
{
	
	int name[] = { CTL_FS, FS_NRFILE };
	int nr_file;
	size_t len = sizeof(nr_file);
	struct __sysctl_args args = {
		.name = name,
		.nlen = sizeof(name)/sizeof(name[0]),
		.oldval = &nr_file,
		.oldlenp = &len,
	};
	if(syscall(SYS__sysctl, &args)){
		printf("error: sysctl: %s\n", strerror(errno));
		return;
	}
	printf("nr_file: %d\n", nr_file);
}

enum{
	ZONE_MOVABLE,
	__MAX_NR_ZONES
};
void main(int argc, char *argv[])
{
	printf("%d\n", __MAX_NR_ZONES);
}
