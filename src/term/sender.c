#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int uart_open(char *filepath) {
	return open(filepath, O_RDWR|O_NOCTTY|O_NDELAY);
}
int uart_send(int fd, char *msg, size_t len) { 
	return write(fd, msg, len);
}
void uart_close(int fd) {
	close(fd);
}

extern yk_hexdump(char *, int);

int main(int argc, char *argv[]) {
	if(argc < 2)
		return 1;
	int fd = uart_open(argv[1]);
	if(fd < 0)  {
		perror("uart_open");
		return 1;
	}
	char buf[1024];
	int bytes = 0;
	while(1) {
		memset(buf, 0, sizeof(buf));
		fgets(buf, sizeof(buf), stdin);
		int len = strlen(buf);
		if(strcasecmp(buf, "exit") == 0)
			break;
		else if(strcasecmp(buf, "stop\n") == 0) {
			memset(buf, 0, sizeof(buf));
			buf[0] = 0x13;
			buf[1] = '\n';
			buf[2] = 0;
		}
		else if(strcasecmp(buf, "start\n") == 0) {
			memset(buf, 0, sizeof(buf));
			buf[0] = 0x11;
			buf[1] = '\n';
			buf[2] = 0;
		}
		yk_hexdump(buf, strlen(buf));
		if(uart_send(fd, buf, strlen(buf)) <= 0) {
			perror("uart_send");
			break;
		}
	}
	uart_close(fd);
}
