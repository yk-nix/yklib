#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <locale.h>

#include <langinfo.h>

#include <libintl.h>

#define _(msgid) gettext(msgid)
#define PACKAGE     "setlocale"
//#define LOCALEDIR   "."

void main(int argc, char *argv[]){
	if(setlocale(LC_ALL, "") == NULL){
		printf("Error: setlocale [AC_ALL] : %s\n", strerror(errno));
		return;
	}

	bindtextdomain(PACKAGE, "locale");
	textdomain(PACKAGE);
	printf(_("hello,yoka\n"));
	
	printf("LC_ADDRESS: %s\n", setlocale(LC_ADDRESS, NULL));
	printf("LC_COLLATE: %s\n", setlocale(LC_COLLATE, NULL));
	printf("D_T_FMT   : %s\n", nl_langinfo(D_T_FMT));
	printf("D_FMT     : %s\n", nl_langinfo(D_FMT));
	printf("DAY_1	  : %s\n", nl_langinfo(DAY_1));
	printf("DAY_7	  : %s\n", nl_langinfo(DAY_7));
	printf("ABDAY_1   : %s\n", nl_langinfo(ABDAY_1));
	printf("ABDAY_7   : %s\n", nl_langinfo(ABDAY_7));
	printf("MON_1	  : %s\n", nl_langinfo(MON_1));
	printf("MON_12    : %s\n", nl_langinfo(MON_12));
	printf("ABMON_1   : %s\n", nl_langinfo(ABMON_1));
	printf("ABMON_12  : %s\n", nl_langinfo(ABMON_12));
	printf("RADIXCHAR : %s\n", nl_langinfo(RADIXCHAR));
	printf("THOUSEP   : %s\n", nl_langinfo(THOUSEP));
	printf("YESEXPR   : %s\n", nl_langinfo(YESEXPR));
	printf("NOEXPR    : %s\n", nl_langinfo(NOEXPR));
	printf("CRNCYSTR  : %s\n", nl_langinfo(CRNCYSTR));
	printf("CODESET   : %s\n", nl_langinfo(CODESET));
	printf("textdomain: %s\n", textdomain(NULL));

}
