#ifndef _STRING_H
#define _STRING_H

void  * my_memset(void *b, int c, int len)
{
	unsigned char *p = b;
	while(len > 0)
	{
		*p = c;
		p++;
		len--;
	}
	return(b);
}

#endif
