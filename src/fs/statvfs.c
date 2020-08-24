#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//#include <unistd.h>

#include <sys/statvfs.h>

void dump_statvfs(struct statvfs *st){
	printf("block size   :  %ld\n", st->f_bsize);
	printf("fragment size:  %ld\n", st->f_frsize);
	printf("block number :  %ld\n", st->f_blocks);
	printf("free blocks  :  %ld\n", st->f_bfree);
	printf("available blk:  %ld\n", st->f_bavail);
	printf("inode number :  %ld\n", st->f_files);
	printf("free inodes  :  %ld\n", st->f_ffree);
	printf("available ino:  %ld\n", st->f_favail);
	printf("filesystem id:  %#x\n", st->f_fsid);
	printf("mount flag   :  %#x\n", st->f_flag);
	printf("name max len :  %d\n", st->f_namemax);
}

void main(int argc, char *argv[]){
	struct statvfs stat;
	if(argc < 2) return;
	if(statvfs(argv[1], &stat)){
		printf("Error: statvfs: %s\n", strerror(errno));
		return;
	}
	dump_statvfs(&stat); 
}
