%{
#include <string.h>

#include "main.h"
#include "yacc.tab.h"

int yywrap();

%}

%x COMMENT

nondigit ([_A-Za-z])
digit ([0-9])
integer ([1-9]({digit}*))
identifier ({nondigit}({digit}|{nondigit})*)
space ([ \t\v\r\n]+)

%%

{identifier} {
	yylval.token = strdup(yytext);
	return IDENTIFIER;
}

{integer} {
	yylval.intval = atoi(yytext);
	return INTEGER;
}

\n {
	lineno++;
}

= {
	yylval.token = strdup(yytext);
	return OPERATOR_EQ;
}


"//" {
	BEGIN COMMENT;
}

<COMMENT>\n {
	BEGIN INITIAL;
	lineno++;
}

. {
}

<COMMENT>. {
}

%%
int yywrap(){
	return 1;
}
