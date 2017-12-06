#ifndef __SYSCALLS_H
#define __SYSCALLS_H
#include<sys/defs.h>
#include<sys/memoryallocator.h>
#include<sys/processLoader.h>
#include<sys/cs.h>
#include<sys/scheduler.h>

void copyPCBContents();
uint64_t getVirtualAddressFromPhysical(uint64_t p);
void copyPML4(uint64_t* newP,uint64_t* p);
void copyPDPE(uint64_t* newP,uint64_t* p);
void copyPDE(uint64_t* newP, uint64_t* p);
void copyPTE(uint64_t* newP, uint64_t* p);
void copyPageTableStructure(uint64_t* pml4Parent);

void copyUserStack();
void copyKernelStack();
void setupMSRs();
void sys_fork();
void copyCurrentProcessIntoAnother();
void cpuSetMSR(uint32_t msr, uint32_t lo, uint32_t hi);
void setSTAR();
void setEFER(uint32_t hi, uint32_t lo);
void syscallHandler();
void setLSTAR(uint32_t hi,uint32_t lo);
uint64_t syscallHndPtr;
uint32_t getLowerHalf(uint64_t num);
uint32_t getHigherHalf(uint64_t num);
void cpuGetMSR(uint32_t msr, uint32_t *lo, uint32_t *hi);
void testSyscall();
void test4();
void setKernGSBase(uint32_t msr,uint32_t hi,uint32_t lo);
int callWrite();
#endif
