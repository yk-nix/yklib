#include <stdio.h>
#include <stdlib.h>

extern int wordCount;
extern FILE *yyin;

void main(int argc, char *argv[]){
        if(argc < 2) return;
        FILE *fp = fopen(argv[1], "r");
        if(fp == NULL) return;
        yyin = fp;
        yylex();
        printf("Number of words: %d\n", wordCount);
}

int yywrap(){
        return 1;
}
