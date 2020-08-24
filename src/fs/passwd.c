#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <termios.h>

#define PASSWD_MAX_LEN	64
#define NAME_MAX_LEN	64

int set_echo_off(){
	struct termios term;
	if(tcgetattr(STDIN_FILENO, &term)){
		printf("Error: set_echo_off: tcgetattr: %s\n", strerror(errno));
		return -1;
	}
	term.c_lflag &= ~(ECHO|ECHOE|ECHOK|ECHONL);
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &term)){
		printf("Error: set_echo_off: tcsetattr: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

int set_echo_on(){
	struct termios term;
	if(tcgetattr(STDIN_FILENO, &term) == -1){
		printf("Error: set_echo_on: tcgetattr: %s\n", strerror(errno));
		return -1;
	}
	term.c_lflag |= (ECHO|ECHOE|ECHOK|ECHONL);
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &term)){
		printf("Error: set_echo_on: tcsetattr: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

int  getnchar(char *buf, int buflen){
	if(buflen <= 0) return 0;
	int i=0;
	char ch;
	while(i<buflen){
		ch = getchar();
		if(ch == '\r' || ch == '\n') break;
		buf[i] = ch;
		i++;
	}
	if(i >= buflen){
		while(getchar() != '\n');
	}
	return i;
}

void main(int argc, char *argv[]){
	char passwd[64], name[64];
	printf("Please input password: ");
	set_echo_off();
	memset(passwd, 0, sizeof(passwd));
	getnchar(passwd, 10);
	set_echo_on();
	memset(name, 0, sizeof(name));
	printf("\nPlease input name: ");
	getnchar(name, 10);
	printf("name: %s    passwd: %s\n", name, passwd);
}
