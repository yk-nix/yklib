#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <libintl.h>

#include <yk_lib.h>
#include <yk_utils.h>

#define roundup(x, base) (x % base == 0 ? x : (x+base-(x%base)))

int main(int argc, char *argv[]) {
	char buf[1024];
	printf("%s\n", setlocale(LC_NUMERIC, NULL));
	printf("%s\n", textdomain(NULL));
	memset(buf, 0, sizeof(buf));
	
}
