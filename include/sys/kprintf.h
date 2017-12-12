#ifndef __KPRINTF_H
#define __KPRINTF_H
#include<sys/defs.h>
void kprintf(const char *fmt, ...);
uint64_t videoMem;
void term_write(char c);
void initialize();
void term_read(char* buffer,int read_ptr,int size);

int valid[10];

volatile uint64_t sbuffer;
volatile uint64_t soffset;
volatile uint64_t read_ptr;


#endif
