#include <sys/scheduler.h>

void initializePCBList(){
	pcbList = (struct task_struct*) kmalloc(4096*10);
	pcbHead = NULL;
}

void yield2(){
	struct task_struct* prev = currentTask;
	currentTask = currentTask->next;
	setProcessSpecificMSRs();
	//TODO Changing the Page tables
	switchCr3(currentTask->pml4P);
	//TODO Flushing the TLB
	//	flushTLB();

	context_switch(prev,currentTask);	
}

void memcpy(void* dest, const void* src, uint64_t size){
	char* s = (char*) src;
	char* d = (char*) dest; 
	int count=0;
	while(count<size){
		*d++=*s++;
		count++;
	}
}

struct task_struct* addPCB(){
	struct task_struct t;
	
	if (pcbHead==NULL){
		*pcbList = t;
		pcbHead = pcbList;
		pcbHead->next = pcbHead;	
	}else{
		pcbHead->next = pcbHead+1;
		pcbHead = pcbHead->next;
		*pcbHead = t;
		pcbHead->next = pcbList;
	}
	return pcbHead;	
}

void removePCB(struct task_struct t){

}

void setProcessSpecificMSRs(){

	uint32_t lo,hi;
        uint32_t *loPtr,*hiPtr;
	loPtr = (uint32_t*) kmalloc(4096);
        hiPtr = (uint32_t*)kmalloc(4096);

	
	hi = getHigherHalf((uint64_t)currentTask);
        lo = getLowerHalf((int64_t)currentTask);
        setKernGSBase(0xC0000102,hi,lo);

        *loPtr=0;
        *hiPtr=0;
        cpuGetMSR(0xC0000102,loPtr,hiPtr);

}
