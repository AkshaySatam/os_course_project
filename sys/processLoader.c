#include<sys/processLoader.h>

extern void switchToRing3(struct task_struct* t);

uint64_t* pml4New;
void dummy(uint64_t addr){
//	uint64_t addr=0x400000;
	uint64_t pmlOff, pdpeOff, pdeOff, pteOff;
	uint64_t *pml4P, *pdpeP, *pdeP, *pteP;
	uint64_t  pdpeAdd, pdeAdd, pteAdd, funcAdd;
	//uint64_t stackAdd;

	pmlOff = getPML4address(addr);
        pdpeOff = getPDPEaddress(addr);
        pdeOff = getPDEaddress(addr);
        pteOff = getPTEaddress(addr);
//	kprintf("PML %d    ",pmlOff);
//	kprintf("PDPE %d   ",pdpeOff);
//	kprintf("PDE %d    ",pdeOff);
//	kprintf("PTE %d   ",pteOff);

        pdpeAdd = getFreePage();
//      kprintf("PDPE %p\n ",pdpe);
        pdeAdd = getFreePage();
//      kprintf("PDE %p\n ",pde);
        pteAdd = getFreePage();
//      kprintf("PT %p\n ",pte);
	funcAdd = getFreePage();	

	//Commenting this code for time-being to load stack lazily
	//stackAdd = getFreePage();

	pml4P = pml4New;
	
	pdpeP = (uint64_t*)(kernmemPtr + (pdpeAdd-pbPtr));
	pdeP = (uint64_t*) (kernmemPtr + (pdeAdd-pbPtr));
	pteP = (uint64_t*) (kernmemPtr + (pteAdd-pbPtr));
	
	*(pml4P+pmlOff) = pdpeAdd|0x7;
	*(pdpeP+pdpeOff) = pdeAdd|0x7;
	*(pdeP+pdeOff) = pteAdd|0x7;

	*(pteP+pteOff) = funcAdd|0x7;
	
	//	Address for Stack. this is dummy
	//		pteOff++;
	//	*(pteP+pteOff) = stackAdd|0x7;	
}

void processPreloading(uint64_t entry){

	uint64_t pml4 = (uint64_t) getFreePage();
	uint64_t physDiff = pml4 - pbPtr;
	uint64_t* pml4V = (uint64_t*)(kernmemPtr+physDiff);
	pml4New = pml4V;
	*(pml4V+511) = pdpePtr|0x7;	
	switchCr3((uint64_t)pml4);

	currentTask->pml4 = (uint64_t)pml4V;
	currentTask->pml4P = pml4;

	//Entering Stack details
	enterVMAdetails(currentTask, 0,0x00000000fffef000,4096*10,1);
	assignUserStack(currentTask,entry);
}

void copyToPrcMem(uint64_t srcMem,uint64_t destMem,uint64_t size, uint64_t entry){
	enterVMAdetails(currentTask, srcMem,destMem,size,0);
//	printVMAdetails(currentTask);
	//TODO Here we copy bytes into the process memory. This will be done by the page-fault handler. Will incorporate this into Page-fault handler later.
	copyBytes(srcMem, destMem,size);
}

void assignUserStack(struct task_struct* task,uint64_t entry){
	//Assign stack pointer
	task->rsp2 = 0x00000000fffeff00;
	task->start_rsp2= task->rsp2;
	task->usrSpcIP = entry;	
}

/*
void initializeVMA(struct task_struct* t){
	t->vmaList = (struct vma*) kmalloc(4096);
	t->vmaCount=0;
	t->vlHead=NULL;
}
*/
void copyBytes(uint64_t src, uint64_t dest, uint64_t size){
	uint64_t* s = (uint64_t*) src;
	uint64_t* d = (uint64_t*) dest; 
	//	char* s = (char*) src;
	//	char* d = (char*) dest; 
	int count=0;
	while(count<(size/8)){
		*d++=*s++;
		count++;
	}
}
	//TODO Address for Stack. this is dummy

void copyBytesReverse(uint64_t src, uint64_t dest, uint64_t size){
	//uint64_t* s = (uint64_t*) src;
	//uint64_t* d = (uint64_t*) dest;
	char* s = (char*) src;
	char* d = (char*) dest; 
	int count=0;
	while(count<size){
		*d-- = *s--;
		count++;
	}
}


void printVMAdetails(struct task_struct* currentTask){
	kprintf("Start V Address %x",currentTask->vmaList->startVAddress);
}

void enterVMAdetails(struct task_struct* currentTask, uint64_t srcMem, uint64_t destMem,uint64_t size, uint64_t type){

	//currentTask = &task2;	
	//This stmt is not reqd as it is put in the process initialization part
	//currentTask->vmaList = (struct vma*) kmalloc(4096);

	//Update the vma count
	currentTask->vmaCount++;
	
	struct vma v;
	v.startVAddress = destMem;
	v.size = size;

	if(type==0)//0 means file backed VMA
	{	v.startPAddress = srcMem;	
		v.type=0;
	}else if (type==1)
	{	v.startPAddress = 0;	
		v.type=1;
	}

	//v.next = NULL;
	if (currentTask->vlHead==NULL){
		*(currentTask->vmaList)= v;
		currentTask->vlHead = currentTask->vmaList;
	} else {
		*(currentTask->vlHead->next) = v;
		currentTask->vlHead = currentTask->vlHead->next; 
	}	
		currentTask->vlHead->next = (struct vma*)(currentTask->vlHead)+1;
		
}
/*
void switchCr3(uint64_t pml4){
	pml4 = pml4 | 0x7;
	enablePaging2(pml4);
	kprintf("New page tables\n");
}


void enablePaging2(uint64_t pml4Ptr){
 __asm__  __volatile__(
                        "movq %0, %%cr3\n"
                        :
                        :"r" (pml4Ptr)
                        :);
}
*/

