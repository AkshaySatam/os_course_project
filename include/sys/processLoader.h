#ifndef _PROCESSLOADER_H_
#define _PROCESSLOADER_H_
#include<sys/kprintf.h>
#include<sys/defs.h>
#include<sys/memoryallocator.h>
#include<sys/cs.h>

void processPreloading(uint64_t entry);
void copyToPrcMem(uint64_t srcMem,uint64_t destMem,uint64_t size,uint64_t entry);
void switchCr3(uint64_t pml4);
void enablePaging2(uint64_t pml4);
void enterVMAdetails(struct task_struct* currentTask, uint64_t srcMem, uint64_t destMem,uint64_t size,uint64_t type);
void printVMAdetails(struct task_struct* currentTask);
void copyBytes(uint64_t src, uint64_t dest, uint64_t size);
void copyBytesReverse(uint64_t src, uint64_t dest, uint64_t size);
void assignUserStack(struct task_struct* task,uint64_t entry);
#endif
