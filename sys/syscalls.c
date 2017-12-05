#include<sys/kprintf.h>
#include<sys/syscalls.h>
#include<sys/cs.h>
//typedef void (*sys_call_ptr_t)(void);
extern void setRFlags(uint64_t rflags);
extern void loadKStack(struct task_struct* currentTask);

char* buf;

void copyString(char* dest,const char* src, int start, int end){
        while(start < end){
                *dest = *(src+start);
                dest++;
                start++;
        }

        *(dest+start)='\0';
}

int writeB (uint64_t fd, char* buf, uint64_t length){
	uint64_t syscall_write = 1;
	uint64_t returnCode=0;
        char buffer[60];
        copyString(buffer,buf,0,length);
	uint64_t  buf_addr = (uint64_t) buffer;
//	kprintf("data is %s\n", buffer);

	 __asm__ volatile (
                        "movq %1, %%rax\n"
                        "movq %2, %%rdi\n"
                        "movq %3, %%rsi\n"
                        "movq %4, %%rdx\n"
                        "syscall\n"
                        "movq %%rax, %0\n"
                        : // output parameters, we aren't outputting anything, no none
                        "=r" (returnCode)
                        : // input parameters mapped to %0 and %1, repsectively
                        "r" (syscall_write), "r" (fd), "r" (buf_addr),"r" (length)
                        //, "m" (syscall_exit),"m" (exit_status)
                        : // registers that we are "clobbering", unneeded since we are calling exit
                        "rax", "rdi","rsi","rdx");
	return returnCode;
}
void setupMSRs(){
	buf =  (char*) kmalloc(4096);
	syscallHndPtr = (uint64_t)&syscallHandler;
	uint32_t lo,hi;
	uint32_t *loPtr,*hiPtr;
	loPtr = (uint32_t*) kmalloc(4096);
	hiPtr = (uint32_t*)kmalloc(4096);
//	kprintf("Function ptr : %x \n",syscallHndPtr);
	lo = getLowerHalf(syscallHndPtr);
	hi = getHigherHalf(syscallHndPtr);
//	kprintf("Low: %x \n",lo);
//	kprintf("High: %x \n",hi);

	//Setting LSTAR
	setLSTAR(hi,lo);
	cpuGetMSR(0xC0000082,loPtr,hiPtr);
//	kprintf("Low LSTAR: %x \n",*loPtr);
//	kprintf("High LSTAR: %x \n",*hiPtr);

	//Setting STAR reg
	uint32_t a = 0x1b<<16;
	uint32_t b =0x8;
	hi = (uint32_t)(a|b);
//	kprintf("Hi- after ORing\n");
	lo=0x00000000;
	setSTAR(hi,lo);
	*loPtr=0;
	*hiPtr=0;
	cpuGetMSR(0xC0000081,loPtr,hiPtr);
//	kprintf("Low STAR: %x \n",*loPtr);
//	kprintf("High STAR: %x \n",*hiPtr);	
	//lo=0x00000001;
	//hi=0x00000000;

	*loPtr=0;
	*hiPtr=0;
	cpuGetMSR(0xC0000080,loPtr,hiPtr);

	//set EFER
	lo = *loPtr;
	lo = lo|0x1;
	hi = *hiPtr;
	setEFER(hi,lo);

	*loPtr=0;
	*hiPtr=0;
	cpuGetMSR(0xC0000080,loPtr,hiPtr);

//	kprintf("Low EFER: %x \n",*loPtr);
//	kprintf("High EFER: %x \n",*hiPtr);	

//	setRFlags(0x00000000);

	//KernGSBase
	currentTask = &task2;
	hi = getHigherHalf((uint64_t)currentTask);
	lo = getLowerHalf((int64_t)currentTask);
	setKernGSBase(0xC0000102,hi,lo);

	*loPtr=0;
	*hiPtr=0;
	cpuGetMSR(0xC0000102,loPtr,hiPtr);

//	kprintf("Low KGSB: %x \n",*loPtr);
//	kprintf("High KGSB: %x \n",*hiPtr);	

//	kprintf("Address of current Process PTR: %x \n",currentTask);			
}

void setKernGSBase(uint32_t msr, uint32_t hi,uint32_t lo){
	__asm__ volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));	
}

void testSyscall(){
//	__asm__ volatile("syscall\n");		
//	kprintf("Back in user space\n");
	buf = "Shree Ganesh";
	writeB(1,buf,12);
//	kprintf("Thank you\n");
//	kprintf("Back in user space\n");
	buf = "Om Namah Shivay";
	writeB(1,buf,20);
	buf = "Keep calm";
	writeB(1,buf,20);
	buf = "and believe";
	writeB(1,buf,12);
}


uint32_t getLowerHalf(uint64_t num){
	uint32_t lo;
	uint64_t tmp = num>>32;

	tmp = tmp<<32;
	lo = (uint32_t)(num-tmp);
	return lo;
}

uint32_t getHigherHalf(uint64_t num){
	uint32_t hi;
	hi = (uint32_t)(num>>32);
	return hi;
}

void cpuSetMSR(uint32_t msr, uint32_t lo, uint32_t hi)
{
	__asm__ volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

void cpuGetMSR(uint32_t msr, uint32_t *lo, uint32_t *hi)
{
   __asm__ volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

void setLSTAR(uint32_t hi,uint32_t lo){
	cpuSetMSR(0xC0000082,lo,hi);
}

void setSTAR(uint32_t hi,uint32_t lo){
	cpuSetMSR(0xC0000081,lo,hi);
}

void setEFER(uint32_t hi,uint32_t lo){
	cpuSetMSR(0xC0000080,lo,hi);
}

void syscallHandler(){
	__asm__ volatile(
			"swapgs\n"
			"movq %%rsp,%%gs:16\n"
			"movq %%gs:8,%%rsp\n"
			"pushq %%gs:16\n"
	//		"push %%rax\n"
			"push %%rbx\n"
			"push %%rcx\n"
			"push %%rdx\n"
			"push %%rdi\n"
			"push %%rsi\n"
			"push %%r8\n"
			"push %%r9\n"
			"push %%r10\n"
			"push %%r11\n"
			"push %%r12\n"
			"push %%r13\n"
			"push %%r14\n"
			"push %%r15\n"
		//	"call callWrite\n"
			"call *sysCallPtr(,%%rax,8)\n"
			"pop %%r15\n"
			"pop %%r14\n"
			"pop %%r13\n"
			"pop %%r12\n"
			"pop %%r11\n"
			"pop %%r10\n"
			"pop %%r9\n"
			"pop %%r8\n"
			"pop %%rsi\n"
			"pop %%rdi\n"
			"pop %%rdx\n"
			"pop %%rcx\n"
			"pop %%rbx\n"
	//		"pop %%rax\n"
			"pop %%gs:16\n"
			"movq %%rsp,%%gs:8\n"
			"movq %%gs:16,%%rsp\n"
			"swapgs\n"
			"sysretq\n"
:::
	);
//kprintf("This should never execute \n");
//	kprintf("RSP of current stack %x \n",currentTask->rsp);
//	kprintf("Stack location of Current thread %x \n ",&(currentTask->kstack[4076]));
//	int i=45;
//	kprintf("Address of the variabe %x\n",&i);
//	__asm__ volatile("sysretq\n");
//	while(1);	
}

/*int callWrite(char* fd,char* buf, uint64_t length){
	kprintf("You are in CallWrite\n");
	return 0;
}*/

void sys_write (){

	//kprintf("You are in sys_write\n");
//	char* buffer= (char*) kmalloc(4096);
	char* buffer;
//	long buf_addr = (long) buffer;
	uint64_t fd, length;
	__asm__ volatile (
	"movq %%rdi,%0\n"
	"movq %%rsi,%1\n"
	"movq %%rdx,%2\n"
	:"=m"(fd),"=m"(buffer),"=m"(length)
	::"rax","rdi","rsi","rdx");

	kprintf("FD: %d\n",fd);
	kprintf("Buffer value %s\n",buffer);
	kprintf("Length: %d\n",length);
}

void sys_read (){}

void (*sysCallPtr[50]) (void)={
	[0]=sys_read,
	[1]=sys_write,
	[3]=sys_fork
};

void sys_fork(){
	copyCurrentProcessIntoAnother();
}

void copyCurrentProcessIntoAnother(){
	
}

int callWrite(){
//	kprintf("You are in CallWrite\n");
	return 0;
}

void test4(){
//	kprintf("You are in test 4\n");
}

