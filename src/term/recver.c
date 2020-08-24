#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


int uart_open(char *filepath) {
	//return open(filepath, O_RDWR|O_NOCTTY|O_NDELAY);
	return open(filepath, O_RDWR|O_NOCTTY);
}
int uart_send(int fd, char *msg, size_t len) { 
	return write(fd, msg, len);
}
int uart_recv(int fd, char *buf, size_t size) {
	return read(fd, buf, size);
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
printf("begin to receiving data:\n");
	while(1) {
		memset(buf, 0, sizeof(buf));
		if((bytes = uart_recv(fd, buf, sizeof(buf))) <= 0) {
			usleep(500000);
		}
		else {
			yk_hexdump(buf, bytes);
			printf("%s", buf);
		}
		if(strcasecmp(buf, "exit") == 0)
			break;
	}
	uart_close(fd);
}
