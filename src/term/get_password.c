#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include <yk_lib.h>
#include <yk_termios.h>

static char *yk_get_password_star(char *buf, size_t size, char ch) {
	char c = 0;;
	char *ret = NULL;
	int fd = fileno(stdin);
	int i = 0;
	struct termios t, old;
	if(!isprint(ch))
		ch = '*';
	memset((void*)&old, 0, sizeof(old));
	if(tcgetattr(fd, &old)) {
		yk_printf(stderr, "yk_get_password_star: tcgetattr: %m\n");
		return NULL;
	}
	memcpy((void*)&t, (void*)&old, sizeof(struct termios));
	t.c_lflag &= ~(ECHO|ECHONL);
	t.c_lflag &= ~(ECHO|ECHONL|ICANON);
	if(tcsetattr(fd, TCSANOW, &t)) {
		yk_fprintf(stderr, "yk_get_password_star: tcsetattr: %m\n");
		return NULL;
	}
	while(1) {
		c = fgetc(stdin);
		if(c == '\n' || c == '\r' || c == '\0')
			break;
		if(!isprint(c))
			continue;	
		if(i < size -1) {
			fprintf(stdout, "%c", ch);
			buf[i++] = c;
		}
	}
	if(tcsetattr(fd, TCSANOW, &old)) {
		yk_printf(stderr, "yk_get_password_star: tcsetattr: %m\n");
		return NULL;
	}
	return buf;
}

static char *yk_get_password_star_raw(char *buf, size_t size, char ch) {
	char c = 0;;
	char *ret = NULL;
	int fd = fileno(stdin);
	int i = 0;
	struct termios t, old;
	if(!isprint(ch))
		ch = '*';
	memset((void*)&old, 0, sizeof(old));
	if(tcgetattr(fd, &old)) {
		yk_printf(stderr, "yk_get_password_star: tcgetattr: %m\n");
		return NULL;
	}
	memcpy((void*)&t, (void*)&old, sizeof(struct termios));
	t.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
	t.c_oflag &= ~OPOST;
	t.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
	t.c_cflag &= ~(CSIZE|PARENB);
	if(tcsetattr(fd, TCSANOW, &t)) {
		yk_fprintf(stderr, "yk_get_password_star: tcsetattr: %m\n");
		return NULL;
	}
	while(1) {
		c = fgetc(stdin);
		if(c == '\n' || c == '\r' || c == '\0')
			break;
		if(!isprint(c))
			continue;	
		if(i < size -1) {
			fprintf(stdout, "%c", ch);
			buf[i++] = c;
		}
	}
	if(tcsetattr(fd, TCSANOW, &old)) {
		yk_printf(stderr, "yk_get_password_star: tcsetattr: %m\n");
		return NULL;
	}
	return buf;
}

static char *yk_get_password_hide(char *buf, size_t size) {
	char *ret = NULL;
	struct termios t;
	int fd = fileno(stdin);
	int len = 0;
	memset((void*)&t, 0, sizeof(t));
	if(tcgetattr(fd, &t)) {
		yk_fprintf(stderr, "yk_get_password_hide: tcgetattr: %m\n");
		return NULL;
	}
	t.c_lflag &= ~ECHO;
	if(tcsetattr(fd, TCSANOW, &t)) {
		yk_fprintf(stderr, "yk_get_password_hide: tcsetattr: %m\n");
		return NULL;
	}
	ret = fgets(buf, size, stdin);
	t.c_lflag |= ECHO;
	if(tcsetattr(fd, TCSANOW, &t)) {
		yk_fprintf(stderr, "yk_get_password_hide: tcsetattr: %m\n");
		return NULL;
	}
	len = strlen(buf);
	if(len < 2)
		return NULL;
	if(buf[len-1] == '\n' || buf[len-1] == '\r')
		buf[len-1] = '\0';
	
	return ret;
}

char *yk_get_password(char *buf, size_t size, int echo_mode) {
	switch(echo_mode) {
	case PASSWORD_ECHO_STAR:
		return yk_get_password_star(buf, size, 0);
	case PASSWORD_ECHO_HIDE:
	default:
		return yk_get_password_hide(buf, size);
	}
}
char *yk_get_password_echo_char(char *buf, size_t size, char ch) {
	return yk_get_password_star(buf, size, ch);
}

#ifdef _GET_PASSWORD_DEBUG
int main(int argc, char *argv[]) {
	char buf[9];
	
	memset(buf, 0, sizeof(buf));
	fprintf(stdout, "please input password: ");
	if(yk_get_password(buf, sizeof(buf), PASSWORD_ECHO_HIDE))
		fprintf(stdout, "\nyour password is: ---%s---\n", buf);
	
	memset(buf, 0, sizeof(buf));
	fprintf(stdout, "please input password: ");
	if(yk_get_password(buf, sizeof(buf), PASSWORD_ECHO_STAR))
		fprintf(stdout, "\nyour password is: ---%s---\n", buf);
	
	memset(buf, 0, sizeof(buf));
	fprintf(stdout, "please input password: ");
	if(yk_get_password_echo_char(buf, sizeof(buf), '@'))
		fprintf(stdout, "\nyour password is: ---%s---\n", buf);
	
	char cache[1024];
	scanf("%s", cache);
	return 0;
}
#endif /* _GET_PASSWORD_DEBUG */
