#include <stdio.h>
/*
 *
 */
unsigned short yk_checksum(char * buff, int count)
{
	register unsigned int sum = 0;

	if(buff == NULL) 
		return -1;
		
	while(count > 1){
		sum    = sum + *(unsigned short *)buff;
		buff  += 2;
		count -= 2;
	}
	if(count > 0)
		sum += *buff;
	while(sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return (unsigned short)~sum;
}
unsigned short yk_checksum16(char * buff, int count)
{
	return yk_checksum(buff, count);
}
