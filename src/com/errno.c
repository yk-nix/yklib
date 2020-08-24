#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void main(int argc, char *argv[])
{
	if(argc < 2) return;
	if(!strcmp(argv[1], "EPERM")) printf("%d\n", EPERM);
	if(!strcmp(argv[1], "EINTR")) printf("%d\n", EINTR);
	if(!strcmp(argv[1], "ELOOP")) printf("%d\n", ELOOP);
	if(!strcmp(argv[1], "EEXIST")) printf("%d\n", EEXIST);
	if(!strcmp(argv[1], "EFAULT")) printf("%d\n", EFAULT);
	if(!strcmp(argv[1], "EFBIG")) printf("%d\n", EFBIG);
	if(!strcmp(argv[1], "EISDIR")) printf("%d\n", EISDIR);
	if(!strcmp(argv[1], "EMFILE")) printf("%d\n", EMFILE);
	if(!strcmp(argv[1], "ENAMETOOLONG")) printf("%d\n", ENAMETOOLONG);
	if(!strcmp(argv[1], "ENFILE")) printf("%d\n", ENFILE);
	if(!strcmp(argv[1], "ENODEV")) printf("%d\n", ENODEV);
	if(!strcmp(argv[1], "ENOENT")) printf("%d\n", ENOENT);
	if(!strcmp(argv[1], "ENOMEM")) printf("%d\n", ENOMEM);
	if(!strcmp(argv[1], "ENOSPC")) printf("%d\n", ENOSPC);
	if(!strcmp(argv[1], "ENXIO")) printf("%d\n", ENXIO);
	if(!strcmp(argv[1], "EOVERFLOW")) printf("%d\n", EOVERFLOW);
	if(!strcmp(argv[1], "EROFS")) printf("%d\n", EROFS);
	if(!strcmp(argv[1], "ETXTBSY")) printf("%d\n", ETXTBSY);
	if(!strcmp(argv[1], "EWOULDBLOCK")) printf("%d\n", EWOULDBLOCK);
	if(!strcmp(argv[1], "EIO")) printf("%d\n", EIO);
	if(!strcmp(argv[1], "EINVAL")) printf("%d\n", EINVAL);
	
	
}
