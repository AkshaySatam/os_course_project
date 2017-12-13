#include <sys/cs.h>
#include <sys/memoryallocator.h>
#include <sys/defs.h>
#include <dirent.h>
#include <sys/kprintf.h>
#include <sys/gdt.h>
#include <sys/syscalls.h>
#include <sys/elfParser.h>
#include <sys/tarfsOps.h>

extern void context_switch(struct task_struct* curr,struct task_struct* next);
extern void switchToRing3(struct task_struct* t);

int pid = 1;
int flag=0;
int counter =0;
void contextSwitch(){
//	create_process(&task);
//	create_process(&task2);

	currentTask = addPCB();
	currentTask->pml4P = pml4;

	task2 = addPCB();
	task2->pml4P = pml4;

	init_process2(task2,(uint64_t)&test3);
	test();
}

// init function
void init_process2(struct task_struct* task2, uint64_t rip){
//	kprintf("Initializing thread 2\n");
	task2->rsp = (uint64_t) &task2->kstack[485];
	task2->start_rsp = (uint64_t) &task2->kstack[500];
	task2->kstack[485] = 0; //r15
	task2->kstack[486] = 0; //r14
	task2->kstack[487] = 0; //r13
	task2->kstack[488] = 0; //r12
	task2->kstack[489] = 0; //r11
	task2->kstack[490] = 0; //r10
	task2->kstack[491] = 0; //r9
	task2->kstack[492] = 0; //r8
	task2->kstack[493] = 0; //rsi
	task2->kstack[494] = 0; //rdi
	task2->kstack[495] = 0; //rdx
	task2->kstack[496] = 0; //rcx
	task2->kstack[497] = 0; //rbx
	task2->kstack[498] = 0; //rax
	task2->kstack[499] = 0; //eflag
	task2->kstack[500] = rip;
	
	initializeVMA(task2);
	//user space info
//	task2.rsp2 = (uint64_t) &task2.ustack[4080];
//	kprintf("First location of user stack pointer %x \n",task2.rsp2);
//	task2.usrSpcIP = (uint64_t) &usrPrFn;

//TODO: We are doing this later. So no need to do it now. But we need to check this.
//	task2.vmaList= (struct vma*) kmalloc(4096);
//	task2.vlHead = NULL;
}

void initializeVMA(struct task_struct* t){
        t->vmaList = (struct vma*) kmalloc(4096);
        t->vmaCount=0;
        t->vlHead=NULL;
}

void usrPrFn(){
	kprintf("In user process\n");
//	testSyscall();
	kprintf("Back in user thread\n");
	//TODO this while (1) is a hack. need to investigate why this is not working
	while(1);
}

void test(){
/*	while(1){
		if(counter<=20){
			counter++;
			kprintf("Exiting thread 1\n");
			yield();
			kprintf("Entering thread 1\n");
		}
	}*/
	while(1){
//		kprintf("Exiting thread 1\n");
		yield2();
//		kprintf("Entering thread 1\n");
	}
}

void test3(){
	uint64_t tarFsHdr, elfHdr;
	//char c[11] = {'b','i','n','/','s','b','u','s','h'};
	char c[11] = {'b','i','n','/','h','e','l','l','o'};
	//char c[11] = {'b','i','n','/','h'};
	tarFsHdr = searchTarfs(c);
	elfHdr = tarFsHdr+512;
//	kprintf("ELF Header %x",elfHdr);
	parseElf(elfHdr);
	switchToRing3(currentTask);
	/*
	TODO: this will be required only when user process returns
	kprintf("Exiting thread 2\n");
	yield();
	kprintf("Entering thread 2\n");
	*/
}
/*
void test2(){
	while(1){
		kprintf("Exiting thread 2\n");
		yield();
		kprintf("Entering thread 2\n");
	}
}
*/
// get the available process ID  
int getProcessID()
{
	for(; pid < MAX_PROCESS; pid++) {
		if(processID[pid] == 0) {
			processID[pid] = 1;
			return pid;
		}
	}
	return -1;
}


//save cr3 VALUE
/*
   static uint64_t getCR3() {
   uint64_t cr3_addr = 0;

// Save current cr3 value 
__asm__ __volatile__("movq %%cr3, %0":: "b"(cr3_addr));  //confirm b or r
return cr3_addr;
}
 */

//create first kernel process
void create_process(struct task_struct* p1){

//	kprintf("Inside create_process\n");
	p1 -> state = RUNNING;
	p1 -> pid = getProcessID();
}	

/*
void yield(){
//	kprintf("In yield\n");
	if(flag==0){
	flag=1;
		switch2(&task,&task2);
	}else {
		flag=0;
		switch2(&task2,&task);
	}
}
*/
void switch2(struct task_struct* curr,struct task_struct* next){
//	kprintf("Inside context_switch\n");
	context_switch(curr,next);
}

void loadNewProcess(char c[]){	
	contextSwitch();
}


