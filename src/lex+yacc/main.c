#include "main.h"

extern FILE *yyin;

int lineno = 0;

void main(int argc, char *argv[]){
	if(argc < 2) return;
	FILE *fp = fopen(argv[1], "r");
	if(fp == NULL){
		printf("can not open file[%s]\n", argv[1]);
		return;
	}
	yyin = fp;
	yyparse();
	fclose(fp);
}
