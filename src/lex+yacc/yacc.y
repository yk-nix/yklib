%{
#include "main.h"
void yyerror(const char *);
extern int yylex(void);

%}

%token <token> IDENTIFIER OPERATOR_EQ 
%token <intval> INTEGER

%%

file: record file
    | record
;

record: IDENTIFIER OPERATOR_EQ INTEGER {
	/*printf("%s %s %d\n", $1.token, $2.token, $3.intval);
	free($1.token);
	free($2.token);*/
	printf("%s %s %d\n", $1, $2, $3);
	free($1);
	free($2);
	
	}
      | IDENTIFIER {
	free($1);
	}
      | INTEGER {
	}
      | IDENTIFIER OPERATOR_EQ IDENTIFIER {
	printf("%s %s %s\n", $1, $2, $3);
	free($1);
	free($2);
	free($3);
	}
      | OPERATOR_EQ {
	free($1);
	}
; 

%%

void yyerror(const char *s){
	printf("line: %d : parser error: %s\n", lineno, s);
	exit(1);
}
