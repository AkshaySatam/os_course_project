#ifndef _TARFSOPS_H_
#define _TARFSOPS_H_
#include <sys/defs.h>

uint64_t searchTarfs(const char* fName);
int  compareStrings(const char* a,const char* b);
void parseTarfs();
int oct2bin(unsigned char *str, int size);
uint64_t decodeTarOctal(unsigned char* data, int size);
#endif
