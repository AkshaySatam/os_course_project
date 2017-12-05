#include<sys/pageFaultHandler.h>
#include<sys/memoryallocator.h>

void handlePageFault(){
	uint64_t faultingAddress = getFaultingAddress();
	kprintf("Faulting address:%x \n",faultingAddress);
	if(isValidAddress(faultingAddress)){
		mapVirtualAddressToPhysical(faultingAddress);
	}else{
		kprintf("Address not mapped in VMA List\n");
	}
//	while(1);
}

short isValidAddress(uint64_t faultingAddress){

	struct vma* v =	currentTask->vmaList;
	uint64_t vmaCount = currentTask->vmaCount;

	while(vmaCount>0){
		if((faultingAddress < (v->startVAddress + v->size))&&(faultingAddress >= (v->startVAddress))){
			return 1;
		}
		v = v+1;
		vmaCount--;				
	}

	return 0;
}

void mapVirtualAddressToPhysical(uint64_t faultingAddress){
	kprintf("Faulting address needs to be mapped\n");
	mapVirtualAddress(faultingAddress);
	kprintf("Mapped the address %x\n",faultingAddress);
}

uint64_t getFaultingAddress(){
	uint64_t faultingAddress;
	__asm__ __volatile__(
			"movq %%cr2,%0\n"
			: "=r"(faultingAddress)
			:
			:
			);
	return faultingAddress;
}
