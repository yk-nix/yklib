#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>

#include <dirent.h>
#include <sys/stat.h>

static char tmpbuf[128];

char *fmode2str(mode){
	memset(tmpbuf, 0, sizeof(tmpbuf));
	if(S_IRUSR & mode) tmpbuf[0] = 'r';
	else tmpbuf[0] = '-';
	if(S_IWUSR & mode) tmpbuf[1] = 'w';
	else tmpbuf[1] = '-';
	if(S_IXUSR & mode) tmpbuf[2] = 'x';
	else tmpbuf[2] = '-';
	if(S_IRGRP & mode) tmpbuf[3] = 'r';
	else tmpbuf[3] = '-';
	if(S_IWGRP & mode) tmpbuf[4] = 'w';
	else tmpbuf[4] = '-';
	if(S_IXGRP & mode) tmpbuf[5] = 'x';
	else tmpbuf[5] = '-';
	if(S_IROTH & mode) tmpbuf[6] = 'r';
	else tmpbuf[6] = '-';
	if(S_IWOTH & mode) tmpbuf[7] = 'w';
	else tmpbuf[7] = '-';
	if(S_IXOTH & mode) tmpbuf[8] = 'x';
	else tmpbuf[8] = '-';
	if(S_ISUID & mode) tmpbuf[2] = 's';
	if(S_ISGID & mode) tmpbuf[5] = 'g';
	if(S_ISVTX & mode) tmpbuf[8] = 't';
	
	return tmpbuf;
}

char *ftype2str(type){
	memset(tmpbuf, 0, sizeof(tmpbuf));
	if(type & DT_DIR) sprintf(tmpbuf, "DIR");
	else if(type & DT_CHR) sprintf(tmpbuf, "char-dev");
	else if(type & DT_REG) sprintf(tmpbuf, "regular");
	else if(type & DT_LNK) sprintf(tmpbuf, "link");
	else if(type & DT_BLK) sprintf(tmpbuf, "block-dev");
	else if(type & DT_SOCK) sprintf(tmpbuf, "socket");
	else sprintf(tmpbuf, "unkown");
	
	return tmpbuf;
}

void lsdir(char *dirpath){
	DIR *dirp;
	struct dirent *dirent;
	int count = 0;

	dirp = opendir(dirpath);
	if(dirp == NULL){
		printf("Error: opendir [%s]: %s\n", dirpath, strerror(errno));
		return;
	}
	
	while(dirent = readdir(dirp)){
		count++;
		printf("%03d  %s	%s\n", 
			count, 
			ftype2str(dirent->d_type),
			dirent->d_name);
	}
	closedir(dirp);
}

void main(int argc, char *argv[]){
	if(argc < 2) return;
	printf("cwd: %s\n", get_current_dir_name());
	
	if(chroot(argv[1])){
		printf("Error: chroot: %s\n", strerror(errno));
		return;
	}
	printf("list current-working-directory:\n");
	lsdir(".");
	printf("list root directory:\n");
	lsdir("/");
	printf("list .. direcotry:\n");
	lsdir("../..");
	//printf("cwd: %s\n", get_current_dir_name());
	if(chdir("..")){
		printf("Error: chdir [.]: %s\n", strerror(errno));
		return;
	}
	if(chdir("..")){
		printf("Error: chdir [..]: %s\n", strerror(errno));
		return;
	}
	
	lsdir(".");
	printf("cwd: %s\n", get_current_dir_name());
}
