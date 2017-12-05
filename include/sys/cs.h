#ifndef cs_h 
#define cs_h
#include<sys/defs.h>
#define KERNEL_STACK_SIZE 4096
#define MAX_PROCESS		100


typedef enum process_state{
	RUNNING,
	SLEEPING,
	ZOMBIE

} process_state;

struct task_struct{
	//Do not add any fields on top of the current field. Add at bottom. Because it will break the OS as the byte-shift mentioned in assembly code will cause unexpected results.
	uint64_t pid;
	uint64_t rsp;			// Kernel thread stack pointer
	uint64_t rsp2;			// User thread stack pointer
	uint64_t usrSpcIP;
	uint64_t kstack[KERNEL_STACK_SIZE];
	uint64_t ustack[KERNEL_STACK_SIZE];
	enum process_state state;
	struct vma* vmaList;
	struct vma* vlHead;
	uint64_t vmaCount;
	uint64_t pml4;
//	uint64_t i;
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
struct task_struct task2;
struct task_struct* currentTask;
struct task_struct *prev,*current,*next;              // The current and the next process in the process list


/*Scheduler functions*/
	void switch2(struct task_struct* next,struct task_struct* prev);
	void create_process(struct task_struct* p1);
	void init_process2();
	void yield();
	void test3();
	void test2();
	void test();
	void usrPrFn();
	void loadNewProcess(char c[]);
#endif

