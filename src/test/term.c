#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <yk_lib.h>
#include <yk_termios.h>

int get_password_test(int argc, char *argv[]) {
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

int tcsetflag_test(int argc, char *argv[]) {
	if(argc < 2)
		return EXIT_FAILED;
	int fd = open(argv[1], O_RDWR|O_NOCTTY|O_NDELAY);
	if(fd < 0) {
		fprintf(stdout, "error: open: %m\n");
		return EXIT_FAILED;
	}
	//yk_term_attr_set(fd, YK_IEXTEN, FLAG_OFF);
	yk_term_attr_dump(fd);
	close(fd);
}

int main(int argc, char *argv[]) {
	return tcsetflag_test(argc, argv);
}
