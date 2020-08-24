
#include <stdio.h>

void yk_hexdump(const char * buff, size_t buffsize)
{
	int lineno = 0, i = 0, idx = 0, linenum;
	linenum = buffsize/16 + 1;
	for(lineno = 0; lineno < linenum; lineno++){
		if(buffsize > 0xffff)
			fprintf(stdout, "%08x ", lineno);
		else 
			fprintf(stdout, "%04x ", lineno);
		
		for(i = 0; i < 16;  i++){
			idx = i + lineno * 16;
			if(idx < buffsize)
				fprintf(stdout, " %02x", (unsigned char)buff[idx]);
			else
				fprintf(stdout, "   ");
			if(i == 7 ) 
				fprintf(stdout, " ");
			if(i == 15) 
				fprintf(stdout, "  | ");
		}
		
		for(i = 0; i < 16;  i++){
			idx = i + lineno * 16;
			if(idx >= buffsize) 
				break;
			fprintf(stdout, "%c", isprint(buff[idx]) ? buff[idx] : '.');
			if(i == 15)
				fprintf(stdout, "|\n");
		}
	}
	fprintf(stdout, "\n");
}

