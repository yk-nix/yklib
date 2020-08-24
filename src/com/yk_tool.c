
#include "yk_tool.h"


/*--------------------------------------------------*/
/*Func Name:     yk_hexdump
/*Descrip  :     dump buffer to check its contents
/*Return   :     void
/*--------------------------------------------------*/
void yk_hexdump(const char * buff, int buffsize)
{
	int lineno = 0, i = 0, idx = 0, linenum;

	if(buffsize <= 0) return;
	
	linenum = buffsize/16 + 1;
	for(lineno = 0; lineno < linenum; lineno++){
		if(buffsize > 0xffff) printf("%08x ", lineno);
		else printf("%04x ", lineno);
		
		for(i = 0; i < 16;  i++){
			idx = i + lineno * 16;
			if(idx < buffsize)
				printf(" %02x", (unsigned char)buff[idx]);
			else
				printf("   ");
			if(i == 7 ) printf(" ");
			if(i == 15) printf("  | ");
		}
		
		for(i = 0; i < 16;  i++){
			idx = i + lineno * 16;
			if(idx >= buffsize) break;
			printf("%c", isprint(buff[idx]) ? buff[idx] : '.');
			if(i == 15) printf("\n");
		}
	}
	printf("\n");
}
