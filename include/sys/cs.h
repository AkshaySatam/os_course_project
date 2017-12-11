#ifndef cs_h 
#define cs_h
#include<sys/defs.h>
#include<sys/fileDescriptor.h>

#define KERNEL_STACK_SIZE 4096
#define MAX_PROCESS		100


typedef enum process_state{
	RUNNING,
	SLEEPING,
	ZOMBIE

} process_state;

//This is for the PCB list
struct task_struct* pcbHead, *pcbList;

struct task_struct{
	//Do not add any fields on top of the current field. Add at bottom. Because it will break the OS as the byte-shift mentioned in assembly code will cause unexpected results.
	uint64_t pid;
	uint64_t rsp;			// Kernel thread stack pointer
	uint64_t rsp2;			// User thread stack pointer
	uint64_t usrSpcIP;
	uint64_t rsp_dummy;
	uint64_t kstack[512];
	uint64_t ustack[512];
//	struct file_descriptor fdArr[128];
	enum process_state state;
	struct vma* vmaList;
	struct vma* vlHead;
	uint64_t vmaCount;
	uint64_t pml4;
	uint64_t pml4P;
	uint64_t start_rsp;			// Kernel thread stack pointer - start
	uint64_t start_rsp2;			// User thread stack pointer - start
	struct task_struct* next;
	short isParent;
	short isChild;
}task_struct;

struct vma{
	uint64_t startVAddress;
	uint64_t size;
	uint64_t startPAddress;
	struct vma* next;
	uint64_t type;
}vma;
void contextSwitch();
int processID[MAX_PROCESS];
struct task_struct task;
struct task_struct* task2;
struct task_struct task3;
struct task_struct* currentTask;
struct task_struct* childTask;
struct task_struct *prev,*current,*next;              // The current and the next process in the process list


	void initializeVMA(struct task_struct* currentTask);
	void switch2(struct task_struct* next,struct task_struct* prev);
	void create_process(struct task_struct* p1);
	void init_process2(struct task_struct* t, uint64_t rip);
//	void yield();
//	void yield2();
	void test3();
	void test2();
	void test();
	void usrPrFn();
	void loadNewProcess(char c[]);
	int getProcessID();
#endif

