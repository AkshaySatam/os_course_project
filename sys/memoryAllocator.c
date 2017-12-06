#include<sys/memoryallocator.h>
#include<sys/kprintf.h>
uint64_t pteOff; 
uint64_t pmlOff;
uint64_t pdpeOff;
uint64_t pdeOff;

uint64_t getPML4address(uint64_t p){
	p = p<<16;
	p = p>>55;
	return p;
}
uint64_t getPDPEaddress(uint64_t p){
	p = p<<25;
	p = p>>55;
	return p;
}
uint64_t getPDEaddress(uint64_t p){
	p = p<<34;
	p = p>>55;
	return p;
}
uint64_t getPTEaddress(uint64_t p){
	p = p<<43;
	p = p>>55;
	return p;
}
uint64_t getOffsetaddress(uint64_t p){
	p = p<<52;
	p = p>>52;
	return p;
}

uint64_t getVirtualAddressFromPhysical(uint64_t p){
        return (kernmemPtr + (p-pbPtr));
}

void mapVirtualAddress(uint64_t addr){
	uint64_t pmlOff, pdpeOff, pdeOff, pteOff;
        uint64_t *pml4P, *pdpeP, *pdeP, *pteP;
        uint64_t  pdpeAdd, pdeAdd, pteAdd, newAdd;
        //uint64_t stackAdd;

        pmlOff = getPML4address(addr);
        pdpeOff = getPDPEaddress(addr);
        pdeOff = getPDEaddress(addr);
        pteOff = getPTEaddress(addr);
        kprintf("PML %d    ",pmlOff);
        kprintf("PDPE %d   ",pdpeOff);
        kprintf("PDE %d    ",pdeOff);
        kprintf("PTE %d   ",pteOff);
	
	pml4P = (uint64_t *) currentTask->pml4;

	//check if pdpe entry is present or not
	if(*(pml4P+pmlOff)==0){
		pdpeAdd = getFreePage();
		*(pml4P+pmlOff) = pdpeAdd|0x7;
	}else{
		pdpeAdd = (*(pml4P+pmlOff))&0xfffffffffffff000;
	}
	pdpeP = (uint64_t*)(kernmemPtr + (pdpeAdd-pbPtr));

	//check if pdpe entry is present or not
	if(*(pdpeP+pdpeOff)==0){
		pdeAdd = getFreePage();
		*(pdpeP+pdpeOff)=pdeAdd|0x7;
	}else{
		pdeAdd = (*(pdpeP+pdpeOff))&0xfffffffffffff000;
	}
	pdeP = (uint64_t*) (kernmemPtr + (pdeAdd-pbPtr));
	
	//check if pte entry is present or not
	if(*(pdeP+pdeOff)==0){
	 	pteAdd = getFreePage();	
		*(pdeP+pdeOff) = pteAdd|0x7;
	}else{
		pteAdd = (*(pdeP+pdeOff))&0xfffffffffffff000;
	}
	pteP = (uint64_t*) (kernmemPtr + (pteAdd-pbPtr));

	newAdd = getFreePage();
	*(pteP+pteOff)=newAdd|0x7;
}

void populatePageDescriptor(uint64_t sAdd,uint64_t  eAdd, uint64_t*  newPhysfree,short isAvailable){
	//kprintf("In populate page Descriptor\n");
	uint64_t s = sAdd;
	pgDescCount=0;
	while(s+4096 <= eAdd){
		pgDescCount++;
		pgDesc->startAddress = s; 
		pgDesc->endAddress=s+4096-1;
		pgDesc->isAvailable = isAvailable;
		//This might generate seg fault for the last entry
		pgDesc->next = pgDesc+1;
		pgDesc->nextFree = pgDesc+1;
		if(s <= (uint64_t) newPhysfree){
			pgDesc->isfree = 0;
		}
		else {
			pgDesc->isfree = 1;
			if(freeFlag==1){
				freeListHead = pgDesc;
				firstFreeLocation = pgDesc;
				freeFlag=0;
	//			kprintf("Free list head: %p\n",freeListHead);
			}
		}

		s+=4096;
		pgDescPrev = pgDesc;
		pgDesc += 1;
	}
}

void testPageDescriptor(uint64_t diff, uint64_t kernmem){
	pgDesc = (struct page_descriptor*) (kernmem+diff);
	while(pgDesc!=NULL){
//		kprintf("Next %p",pgDesc->next);
		pgDesc=pgDesc->next;
	}
}

void testKmalloc(){
	uint64_t* c;
	for (int i =1;i<=10;i++){
		c = kmalloc(i*4096);
		*c = 24*i;
		kprintf("C:%d        ",*c);
	}
//	c=kmalloc(9000);
//	*c=48;
//	c=kmalloc(4096);
//	*c=90;
	//kprintf("%d\n",c);
}

void initializeMemory (uint64_t totalMemory, uint64_t* startIndices, uint64_t*  endIndices, void * physbase, void * physfree, uint64_t counter,uint64_t kernmem)
{

	pbPtr = (uint64_t)physbase;
	kernmemPtr=kernmem;

	// TODO clean up memory from physfree +1 to end by making it to all zeroes using memset.

	// Size of the page_DescArray
	int pgDescSize = (int) totalMemory/4096;
//	kprintf("Total PD size %d\n ",pgDescSize);

	// calculate the size occupied by  page_DescArray
/*	uint64_t pgDescSizeInBytes = pgDescSize * sizeof(struct page_descriptor);
	int physfreeShift = pgDescSizeInBytes/8;

	// get new physfree by adding current physfree and page_desc_size
	uint64_t* newPhysfree = (uint64_t *) (physfree + physfreeShift+1); 
//	kprintf("New physfree %x \n", newPhysfree);
*/
	
	struct page_descriptor* p = (struct page_descriptor*)physbase;

	for(int i =0;i<=pgDescSize;i++){
		p+=1;
	}	
	uint64_t* newPhysfree = (uint64_t *) p; 
	// point pgDesc to newPhysfree.
	pgDesc =  (struct page_descriptor*) physfree;
//	kprintf("Start of page descriptor %x \n", pgDesc);

	int i =0;
	uint64_t sAdd,eAdd;	

	freeFlag=1;
	while(i<counter-1){
		sAdd = startIndices[i];
		eAdd = endIndices[i];
		populatePageDescriptor(sAdd,eAdd,newPhysfree,1);
		populatePageDescriptor(eAdd,startIndices[i+1]-1,newPhysfree,0);
		i++;
	}

	
	//mapping last memory addresses
	sAdd = startIndices[i];
	eAdd = endIndices[i];
	populatePageDescriptor(sAdd,eAdd,newPhysfree,1);

	//Make the last pointer as NULL	
	pgDescPrev -> next = NULL;

	excludeUnavailableMemory((uint64_t)physfree);
	pagingSetup();
	mapKernel(kernmem,(uint64_t)physbase,(uint64_t) newPhysfree);
	mapVideoCard(0xffffffff800b8000);
	//mapVideoCard(0xffffffff80240000);
//	kprintf("Kernel and video mapping done\n");
//	mapEntireMemory(0xffffffff80000000,(uint64_t) newPhysfree,(uint64_t)physfree);
	mapEntireMemory((uint64_t) newPhysfree);
	enablePaging(pml4Ptr);
	kprintf("Paging done\n");
	remapNextPointers((uint64_t) physbase,kernmem,(uint64_t)physfree,(uint64_t)newPhysfree);
//	testPageDescriptor((uint64_t)physfree-(uint64_t)physbase,kernmem);
	testKmalloc();
}


void excludeUnavailableMemory(uint64_t physfree){

	struct page_descriptor*	p =  (struct page_descriptor*) physfree;
	struct page_descriptor*	prev =  NULL;
	while(p!=NULL){
		if(p->isAvailable==1){
			prev=p;
			p=p->next;
		}
		else{
			while((p!=NULL)&&(p->isAvailable==0)){
				p=p->next;
			}
			prev->nextFree=p;			
		}
	}
}

/*
int my_ceil(int num){
	int inum = (int)num;
	if (num == inum) {
		return inum;
	}
	return inum + 1;
}*/

uint64_t* kmalloc (uint64_t memorySize){
	int noPages = memorySize%4096==0?(int) memorySize/4096:(int)(memorySize/4096)+1;
//	int noPages =1;
	uint64_t pAddress = getFreePages(noPages);
	uint64_t physDiff = pAddress - pbPtr;
	return (uint64_t*)(kernmemPtr+physDiff);	
}

void mapEntireMemory(uint64_t newPhysfree){

	struct page_descriptor* ptr = firstFreeLocation;
	uint64_t i;
	uint64_t *p3,*p4;
	p4 = (uint64_t*) pte;
	p3 = (uint64_t*) pde;
	//p2 = (uint64_t*) pdpe;
	//p1 = (uint64_t*) pml4;

	while( (uint64_t) ptr <= newPhysfree){
		if(pteOff==512)
		{
			pte = getFreePage();
			p4 = (uint64_t*) pte;
			pteOff = 0;
			pdeOff++;
			//TODO Commend this 
	/*		if(pdeOff==512){
				pde = getFreePage();
				p3 = (uint64_t*) pde;
				pdeOff=0;
				pdpeOff++;
				if(pdpeOff==512){
					pdpe = getFreePage();
					p2 = (uint64_t*) pdpe;
					pdpeOff=0;
					pmlOff++;
					if(pmlOff==512){
						break;	
					}
					*(p1+pmlOff)=pdpe|0x3;
				}
				*(p2+pdpeOff)=pde|0x3;
			}*/
			*(p3+pdeOff)=pte|0x7;
		}

		i = ptr->startAddress;
		*(p4+pteOff)= i | 0x7;
		pteOff++;
		ptr+=1;			
	}
	return;	
}

/*
void mapEntireMemory(uint64_t startAddress, uint64_t newPhysfree,uint64_t physfree){
	struct page_descriptor* ptr = (struct page_descriptor*) physfree;
	uint64_t i;
	uint64_t *p1,*p2,*p3,*p4;
	uint64_t* p = (uint64_t*)pml4;

	pmlOff = getPML4address(startAddress);
	pdpeOff = getPDPEaddress(startAddress);
	pdeOff = getPDEaddress(startAddress);
	pteOff = getPTEaddress(startAddress);
	
	//Putting entry in PML4
	*(p+pmlOff)= pdpe|0x7;
	
	//Putting entry in PDPE
	p = (uint64_t*)pdpe;
	*(p+pdpeOff)= pde|0x7;

	//Putting entry in PDE
	p = (uint64_t*)pde;
	*(p+pdeOff)=pte|0x7;
	
	//This will be loaded into CR3	
	pml4Ptr = pml4|0x7;

	p4 = (uint64_t*) pte;
	p3 = (uint64_t*) pde;
	p2 = (uint64_t*) pdpe;
	p1 = (uint64_t*) pml4;
			 	
	while( (uint64_t) ptr <= newPhysfree){
		if(pteOff==512)
		{
			pte = getFreePage();
			p4 = (uint64_t*) pte;
			pteOff = 0;
			pdeOff++;
			
			if(pdeOff==512){
				pde = getFreePage();
				p3 = (uint64_t*) pde;
				pdeOff=0;
				pdpeOff++;
				if(pdpeOff==512){
					pdpe = getFreePage();
					p2 = (uint64_t*) pdpe;
					pdpeOff=0;
					pmlOff++;
					if(pmlOff==512){
						break;	
					}
					*(p1+pmlOff)=pdpe|0x7;
				}
				*(p2+pdpeOff)=pde|0x7;
			}
			*(p3+pdeOff)=pte|0x7;
		}

		i = ptr->startAddress;
		*(p4+pteOff)= i | 0x7;
		pteOff++;
		ptr+=1;			
	}
return;	
}
*/

void remapNextPointers(uint64_t physbase,uint64_t kernmem,uint64_t physfree,uint64_t newPhysfree){
	uint64_t physDiff = physfree - physbase;
	pgDesc = (struct page_descriptor*) (kernmem+physDiff);
	uint64_t diff = newPhysfree - physbase; 
//	int count =0;
	uint64_t diff2,diff3;
	while((uint64_t)pgDesc<=(kernmem+diff)){
	//while(pgDesc!=NULL){
		pgDesc->next=pgDesc+1;

		diff2 = (uint64_t)(pgDesc->nextFree) - physbase;
		pgDesc->nextFree = (struct page_descriptor*) (kernmem + diff2);

	//	kprintf("Count: %d Next:%p \n",count,pgDesc->next);
//		count++;

		pgDesc=pgDesc->next;
	}
//	pgDesc->next= NULL;
	diff3 = (uint64_t)freeListHead-physbase;	
	freeListHead = (struct page_descriptor*) (kernmem + diff3);
	firstFreeLocation = (struct page_descriptor*)(kernmem + ((uint64_t) firstFreeLocation-physbase));	
}

void memset(uint64_t start, uint64_t size, uint64_t value){
	char* c;
	for(int i=0;i<size;i++){	
		 c = (char*)(start+i);
		*c = 0;
	}
}

uint64_t getFreePage(){
	uint64_t headV;	
	uint64_t head =  freeListHead->startAddress;
	freeListHead->isfree=0;
	freeListHead = freeListHead->nextFree;
	headV = getVirtualAddressFromPhysical(head);
	memset(headV,4096,0);
	return head;
}

uint64_t getFreePages(int count){	
	uint64_t headV;	
	uint64_t head =  freeListHead->startAddress;
	int i=1;
	while(i<=count){
		freeListHead->isfree=0;
		freeListHead = freeListHead->nextFree;
		i++;
	}
	headV = getVirtualAddressFromPhysical(head);
	memset(headV,4096*count,0);
	return head;
}

void pagingSetup(){
	pml4 = getFreePage();
//	kprintf("PML4 %p\n ",pml4);
	pdpe = getFreePage();
//	kprintf("PDPE %p\n ",pdpe);
	pde = getFreePage();
//	kprintf("PDE %p\n ",pde);
	pte = getFreePage();	
//	kprintf("PT %p\n ",pte);
}
	
void enablePaging(uint64_t pml4Ptr){
	/* __asm__  __volatile__(
                        "movq %0, %%rax\n"
                        "movq %%rax, %%cr3\n"
                        "movq %%cr0, %%rax\n"
			"or $0x80000001, %%eax\n"
                        "movq %%rax, %%cr0\n"
			:
			:
                        "r" (pml4Ptr)
			:
                        "rax");*/
	 __asm__  __volatile__(
                        "movq %0, %%cr3\n"
			:
			:"r" (pml4Ptr)
                        :);
	
}

void mapKernel(uint64_t kernmem,uint64_t physbase, uint64_t newPhysfree){
	pmlOff = getPML4address(kernmem);
	pdpeOff = getPDPEaddress(kernmem);
	pdeOff = getPDEaddress(kernmem);
	pteOff = getPTEaddress(kernmem);
	pdpePtr = pdpe;
	uint64_t i = physbase;

	//Putting entry in PML4
	uint64_t* p = (uint64_t*)pml4;
	*(p+pmlOff)= pdpe|0x7;

	//Putting entry in PDPE
	p = (uint64_t*)pdpe;
	*(p+pdpeOff)= pde|0x7;

	//Putting entry in PDE
	p = (uint64_t*)pde;
	*(p+pdeOff)=pte|0x7;

	//Putting entries in PTE
	p = (uint64_t*)pte;	

	while(i <= newPhysfree){
		*(p+pteOff)= i | 0x7;
		pteOff++;
		i+=0x1000;	
	}		
	pml4Ptr = pml4|0x7;
}


void mapVideoCard(uint64_t videoMemory){

	//uint64_t pdpeV = getFreePage();
	//uint64_t pdeV = getFreePage();
	uint64_t pteV = getFreePage();	

	//uint64_t pmlOffV = getPML4address(videoMemory);
	//uint64_t pdpeOffV = getPDPEaddress(videoMemory);
	uint64_t pdeOffV = getPDEaddress(videoMemory);
	uint64_t pteOffV = getPTEaddress(videoMemory);
	uint64_t physMem = 0xb8000;

	uint64_t i = physMem;

	//uint64_t* p = (uint64_t*)pml4;
	// *(p+pmlOffV)= pdpeV|0x3;
	
	//Putting entry in PDPE
	//p = (uint64_t*)pdpeV;
	// *(p+pdpeOffV)= pdeV|0x3;

	//Putting entry in PDE
	uint64_t* p = (uint64_t*)pde;
	*(p+pdeOffV)=pteV|0x7;
		
	//Putting entries in PTE
	p = (uint64_t*)pteV;	

	while(i <= (physMem + 500)){
		*(p+pteOffV)= i | 0x7;
		pteOffV++;
		i+=0x1000;	
	}		
}

void setVirtualAddressForPhysicalMemory(uint64_t pmloff,uint64_t pdpeoff,uint64_t pdeoff,uint64_t pteoff){
	/*uint64_t p4 = pteoff<<12;
	uint64_t p3 = pdeoff<<21;
	uint64_t p2 = pdpeoff<<30;
	uint64_t p1 = pmloff<<39;
	uint64_t p0 = 0xffff000000000000;
	*///videoMem = p4|p3|p2|p1|p0;	
}
