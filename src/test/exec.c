#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
	int ret;
	ret = execl("/usr/bin/bash", "bash", NULL);
	fprintf(stderr, "execl: %m\n");
	return ret;
}
