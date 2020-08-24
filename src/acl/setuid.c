#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>


void main(int argc, char *argv[]){
	int ruid, euid, suid;
	if(getresuid(&ruid, &euid, &suid)){
		printf("error: getresuid: %s\n", strerror(errno));
	}
	printf("start     ruid:%d  euid:%d  suid:%d\n", ruid, euid, suid);

	if(setuid(1000)){
		printf("error: setuid[yui]: %s\n", strerror(errno));
		return;
	} 
	if(getresuid(&ruid, &euid, &suid)){
		printf("error: getresuid: %s\n", strerror(errno));
		return;
	}
	printf("-->yui   ruid:%d  euid:%d  suid:%d\n", ruid, euid, suid);
	
	if(setuid(1002)){
		printf("error: setuid[lisa]: %s\n", strerror(errno));
		return;
	} 
	if(getresuid(&ruid, &euid, &suid)){
		printf("error: getresuid: %s\n", strerror(errno));
		return;
	}
	printf("-->lisa  ruid:%d  euid:%d  suid:%d\n", ruid, euid, suid);
	
	
	if(setuid(1003)){
		printf("error: setuid[samba]: %s\n", strerror(errno));
		return;
	} 
	if(getresuid(&ruid, &euid, &suid)){
		printf("error: getresuid: %s\n", strerror(errno));
		return;
	}
	printf("-->samba  ruid:%d  euid:%d  suid:%d\n", ruid, euid, suid);
	
	if(setuid(0)){
		printf("error: setuid[root]: %s\n", strerror(errno));
		return;
	} 
	if(getresuid(&ruid, &euid, &suid)){
		printf("error: getresuid: %s\n", strerror(errno));
		return;
	}
	printf("-->root   ruid:%d  euid:%d  suid:%d\n", ruid, euid, suid);
}
