#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct{
	char *token;
	char *strval;
	int  intval;
}yytype_t;

#define YYSTYPE yytype_t

extern int lineno;

#endif /* _MAIN_H_ */
