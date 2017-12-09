#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_
#include <sys/cs.h>
#include <sys/kprintf.h>
#include <sys/defs.h>
#include <sys/memoryallocator.h>
#include <sys/syscalls.h>

extern void context_switch(struct task_struct* prev, struct task_struct* current);
//struct task_struct* addPCB(struct task_struct t);
struct task_struct* addPCB();
void removePCB(struct task_struct t);
void yield2();
void initializePCBList();
void setProcessSpecificMSRs(struct task_struct* t);
#endif

