#ifndef _MEMSET_H
#define _MEMSET_H

/*
int _port;
uint32_t startl = 0;
uint32_t starth = 100;
void port_rebase(hba_port_t *port, int portno);
hba_mem_t *abar;
char bufc[1024] = "T"; //his is the test buffer";
uint32_t *buf = (uint32_t *)bufc;
char buf2c[1024];
uint32_t *buf2 = (uint32_t *)buf2c;
*/

int read(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint32_t*  buf);
int write(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint32_t* buf);

void  * memset_tmp(void *b, int c, int len)
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

