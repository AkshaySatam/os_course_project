#include<sys/kprintf.h>
#include<sys/syscalls.h>
#include<sys/cs.h>
#include <sys/interrupts.h>

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
	//No need of below stmt as this will be always executed on the current task
	//currentTask = &task2;

	//MOved this in another function
/*	hi = getHigherHalf((uint64_t)currentTask);
	lo = getLowerHalf((int64_t)currentTask);
	setKernGSBase(0xC0000102,hi,lo);

	*loPtr=0;
	*hiPtr=0;
	cpuGetMSR(0xC0000102,loPtr,hiPtr);*/

//	kprintf("Low KGSB: %x \n",*loPtr);
//	kprintf("High KGSB: %x \n",*hiPtr);	

//	kprintf("Address of current Process PTR: %x \n",currentTask);			
}

void setKernGSBase(uint32_t msr, uint32_t hi,uint32_t lo){
	__asm__ volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));	
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
//			"pushq %%gs:16\n"
//TODO Fork wont work.			"push %%rax\n" // Ignore: Dont push/pop rax as it will be used to return the value
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
			"movq %%rsp,%%r15\n"
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
//			"pop %%rax\n"	
			//"pop %%gs:16\n"
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

	kprintf("%s",buffer);
//	kprintf("\n");
	__asm__ volatile (
			"movq %0,%%rax\n"
			::"r"(length):);
}

void sys_read (){
	//	kprintf("In sys_read\n");

	char* buffer;
	uint64_t fd, length;
	__asm__ volatile (
			"movq %%rdi,%0\n"
			"movq %%rsi,%1\n"
			"movq %%rdx,%2\n"
			:"=m"(fd),"=m"(buffer),"=m"(length)
			::"rax","rdi","rsi","rdx");

	//kprintf("FD %d\n",fd);
	//kprintf("Buffer %s\n",buffer);
	//kprintf("length %d\n",length);


	if(fd == 0){	
		while(1){
			if(valid[read_ptr] == 1){
				break;
			}
		}
		term_read(buffer,read_ptr,length);
	}
	else{
		length = fileRead(fd,buffer,length);
	}
	//   kprintf("%s\n",buffer);
	__asm__ volatile (
			"movq %0,%%rax\n"
			:
			:"r"(length)
			:);
	//kprintf("Out of sys_read\n");
}

void sys_open(){

        char* buffer;
	uint64_t flags;
	volatile uint64_t l=1;

        __asm__ volatile (
        "movq %%rdi,%0\n"
        "movq %%rsi,%1\n"
        :"=m"(buffer), "=m"(flags)
        ::"rax","rdi","rsi");

//	kprintf("Open: %s",buffer);	
//	kprintf("Open flags: %d",flags);	
     
	l = open(buffer,flags); 

//	kprintf("File descriptor: %d",l);	

	 __asm__ volatile (
        "movq %0,%%rax\n"
        :
	:"r"(l):"rax");	
}

void sys_close(){
	uint64_t fd;
	uint64_t l;

        __asm__ volatile (
        "movq %%rdi,%0\n"
        :"=m"(fd)
        ::"rax","rdi");

//	kprintf("FD: %d",fd);	
     
	l = close(fd); 

	 __asm__ volatile (
        "movq %0,%%rax\n"
        :
	:"r"(l)
	:"rax");	
}

void sys_sleep(){
        char* buffer;
        __asm__ volatile (
        "movq %%rdi,%0\n"
        :"=m"(buffer)
        ::"rax","rdi");

	int startTime = currentTime;
	int sleepTime = (int)(atoi(buffer)/1000);
	uint64_t l =0;

	while(1){
		if(currentTime >= (startTime + sleepTime)){
			break;
		}
	}	
	//kprintf("Sleep time: %s",buffer);	

        __asm__ volatile (
        "movq %0,%%rax\n"
        :
	:"m"(l):);
}

int atoi(char *str)
{
    int res = 0;
//This code is referred from geeksforgeeks.com  
    for (int i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';
  
    return res;
}

void sys_ps(){
	int l=0;
	struct task_struct* t = pcbList;
	int count =0;
	kprintf("PID\n");
	while(count < pcbCount){
		kprintf("%d\n",t->pid);
		t++;	
		count++;
	}
        __asm__ volatile (
        "movq %0,%%rax\n"
        :
	:"m"(l):);
}

void (*sysCallPtr[50]) (void)={
	[0]=sys_read,
	[1]=sys_write,
	[2]=sys_fork,
	[3]=sys_sleep,
	[4]=sys_open,
	[5]=sys_close,
	[6]=sys_ps
};

void sys_fork(){
	copyCurrentProcessIntoAnother();
//	kprintf("Exiting fork : PCB number - %d \n",currentTask->pid);
//	yield2();
}

void copyPCBContents(){
	struct vma* vl = currentTask->vmaList;
	uint64_t vmaCount = currentTask->vmaCount;
	while(vmaCount>0){
		struct vma v;
		v.startVAddress = vl->startVAddress;
		v.size = vl->size;
		v.startPAddress = vl->startPAddress;
		v.type=vl->type;
	
		if (childTask->vlHead==NULL){
			*(childTask->vmaList)= v;
			childTask->vlHead = childTask->vmaList;
		} else {
			*(childTask->vlHead->next) = v;
			childTask->vlHead = childTask->vlHead->next;
		}
		childTask->vlHead->next = (struct vma*)(childTask->vlHead)+1;
		childTask->vmaCount++;
	
                vl = vl+1;
                vmaCount--;
        }
	
}

void copyUserStack(){

	//TODO This stack is being assigned on parent process's address space. Should change this.
	//enterVMAdetails(currentTask,0,0x409000,4096,1);
	//uint64_t size = currentTask->start_rsp2 - currentTask->rsp2;
	//copyBytesReverse(currentTask->start_rsp2,0x409ff0,size);

	// set the RSPs of child user stacks
	childTask->rsp2 = currentTask->rsp2;
	childTask->start_rsp2 = 0x409ff0;
}

void copyKernelStack(){
	
	copyBytes((uint64_t) &currentTask->kstack[0], (uint64_t)&childTask->kstack[0],4096);
	
	uint64_t rsp_dummy;

	__asm__ __volatile__ (
		"movq %%r15, %0;"
		: "=r" (rsp_dummy)
		:
		:
	);


//	childTask->rsp = (uint64_t) &(childTask->kstack[500]);
//	uint64_t size = currentTask->start_rsp - currentTask->rsp_dummy;
//	copyBytesReverse(currentTask->start_rsp,childTask->rsp,size);
	//childTask->rsp = (childTask->rsp) - (8*size);

	//uint64_t i = ((uint64_t)childTask->kstack + (rsp_dummy - (uint64_t)currentTask->kstack))/8;

	uint64_t i =  (rsp_dummy - (uint64_t)currentTask->kstack)/8;
	
	//These method directly manipulate the Rax values on stack. Commenting for time being.
	uint64_t j = i;
	updateRAX(&currentTask->kstack[j+13],childTask->pid);
	updateRAX(&childTask->kstack[j+13],0);

	i--;
	childTask->kstack[i--]= (uint64_t)((&syscallHandler)+53);
	childTask->kstack[i--]=0;//eflag
	childTask->kstack[i--]=0;//rax
	childTask->kstack[i--]=0;//rbx
	childTask->kstack[i--]=0;//rcx
	childTask->kstack[i--]=0;//rdx
	childTask->kstack[i--]=0;//rdi
	childTask->kstack[i--]=0;//rsi
	childTask->kstack[i--]=0;//r8
	childTask->kstack[i--]=0;//r9
	childTask->kstack[i--]=0;//r10
	childTask->kstack[i--]=0;//r11
	childTask->kstack[i--]=0;//r12
	childTask->kstack[i--]=0;//r13
	childTask->kstack[i--]=0;//r14
	childTask->kstack[i]=0;  //r15			
	
	//set the RSPs of child kernel stacks
	childTask->rsp = (uint64_t) &(childTask->kstack[i]);
	childTask->start_rsp = 	(uint64_t) &(childTask->kstack[500]);
}

void updateRAX(uint64_t* add , uint64_t value){
	*add = value;
}

void copyParentStacks(){
        copyUserStack();
        copyKernelStack();
}

void copyCurrentProcessIntoAnother(){
	childTask = addPCB(); 	
	initializeVMA(childTask);
	currentTask->isParent = 1;
	childTask->isChild = 1;
	copyPageTableStructure((uint64_t*) getVirtualAddressFromPhysical(currentTask->pml4P));
	copyParentStacks();	
	copyPCBContents();
}

void copyPML4(uint64_t* newP,uint64_t* p){
	uint64_t p1,p1V,p2,p2V;
	for(int i=0;i<511;i++){
		if(*(p+i)!=0){
			p1 = getFreePage();
			p1V = getVirtualAddressFromPhysical(p1);
			*(newP+i)=p1|0x7;
			
			p2 = (*(p+i))&0xfffffffffffff000;
			p2V = getVirtualAddressFromPhysical(p2);
			copyPDPE((uint64_t*)p1V,(uint64_t*)p2V);
		}
	}
	*(newP+511) = *(p+511);	
}

void copyPDPE(uint64_t* newP,uint64_t* p){
	uint64_t p1,p1V,p2,p2V;
	for(int i=0;i<512;i++){
		if(*(p+i)!=0){
			p1 = getFreePage();
			p1V = getVirtualAddressFromPhysical(p1);
			*(newP+i)=p1|0x7;
			
			p2 = (*(p+i))&0xfffffffffffff000;
			p2V = getVirtualAddressFromPhysical(p2);
			copyPDE((uint64_t*)p1V,(uint64_t*)p2V);
		}
	}	
}

void copyPDE(uint64_t* newP, uint64_t* p){
	uint64_t p1,p1V,p2,p2V;
	for(int i=0;i<512;i++){
		if(*(p+i)!=0){
			p1 = getFreePage();
			p1V = getVirtualAddressFromPhysical(p1);
			*(newP+i)=p1|0x7;
			
			p2 = (*(p+i))&0xfffffffffffff000;
			p2V = getVirtualAddressFromPhysical(p2);
			copyPTE((uint64_t*)p1V,(uint64_t*)p2V);
		}
	}	
}

void copyPTE(uint64_t* newP, uint64_t* p){
	
	uint64_t p1;
	for(int i=0;i<512;i++){
		if(*(p+i)!=0){
			p1 = (*(p+i)) & 0xfffffffffffff000;
			//TODO should change the perm to 0x805
			//TODO also change the parent permissions when you are sure that both the threads return to User space
			*(newP+i) = p1|0x805;
		}
	}	
}

void copyPageTableStructure(uint64_t* pml4Parent){
	uint64_t pml4,pml4V;

	pml4 = getFreePage();
	childTask->pml4P = pml4;
	//TODO this is a right way of doing things but commenting for now
	//childTask->pml4P = currentTask->pml4P;

	pml4V = getVirtualAddressFromPhysical(pml4);
	copyPML4((uint64_t*)pml4V, (uint64_t*)pml4Parent);
}

int callWrite(){
//	kprintf("You are in CallWrite\n");
	return 0;
}

void test4(){
//	kprintf("You are in test 4\n");
}

