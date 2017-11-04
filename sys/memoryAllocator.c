#include<sys/memoryallocator.h>
#include<sys/kprintf.h>

void populatePageDescriptor(uint64_t sAdd,uint64_t  eAdd, uint64_t*  newPhysfree){
//kprintf("In populate page Descriptor\n");
	uint64_t s = sAdd;
	struct page_descriptor pD;
	while(s+4096 < eAdd){
		pD = *pgDesc;
		pD.startAddress = s;
		pD.endAddress = s + 4096 - 1;
		if(s <= (uint64_t) newPhysfree){
			pD.next = NULL;
			pD.isfree = 0;
			kprintf("%d\n",pD.isfree);
		}
		else {
			//This might generate seg fault for the last entry
			pD.next = pgDesc+1;
			pD.isfree = 1;
			kprintf("%d",pD.isfree);
		}
		s+=4096;
		pgDesc += 1;
	}
}
void initializeMemory (uint64_t totalMemory, uint64_t* startIndices, uint64_t*  endIndices, void * physbase, void * physfree, uint64_t counter)
{
	
	// TODO clean up memory from physfree +1 to end by making it to all zeroes using memset.
	
	// Size of the page_DescArray
	int pgDescSize = (int) totalMemory/4096;
	kprintf("Total PD size %d\n ",pgDescSize);

	// calculate the size occupied by  page_DescArray
	uint64_t pgDescSizeInBytes = pgDescSize * sizeof(struct page_descriptor);
	int physfreeShift = pgDescSizeInBytes/8;

	// get new physfree by adding current physfree and page_desc_size
	//uint64_t* p = (uint64_t*)physfree;
	//uint64_t* newPhysfree = (uint64_t*)(*p + pgDescSizeInBytes);
	uint64_t* newPhysfree = (uint64_t *) (physfree + physfreeShift+1); 
	kprintf("New physfree %p \n", newPhysfree);
	
	// point pgDesc to newPhysfree.
	pgDesc =  (struct page_descriptor*)newPhysfree;
	kprintf("Start of page descriptor %p \n", pgDesc);
	
	int i =0;
	uint64_t sAdd,eAdd;	

	while(i<counter){
		sAdd = startIndices[i];
		eAdd = endIndices[i];
		populatePageDescriptor(sAdd,eAdd,newPhysfree);
		i++;
	}
		
//	pgDesc =  (struct page_descriptor*)newPhysfree;
//	struct page_descriptor pD = *pgDesc;
//	pD.
	
/*	for(int i=0;i<pdDescSize;i++){
	
	}
*/	
//TODO link freeList correctly	freeListHead = &pgDesc[0];
}
