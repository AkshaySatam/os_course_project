#include<sys/pageFaultHandler.h>
#include<sys/memoryallocator.h>

void handlePageFault(){
	uint64_t a;
	uint64_t faultingAddress = getFaultingAddress();
	kprintf("Faulting address:%x \n",faultingAddress);
	if(isValidAddress(faultingAddress)){
		if(cowBitSet(faultingAddress)){
			//TODO you might need to kmalloc
			a = (uint64_t) kmalloc(4096);
			copyBytes((faultingAddress & 0xfffffffffffff000),(uint64_t)a,4096);
		}else{
			mapVirtualAddressToPhysical(faultingAddress);
		}
	}else{
		kprintf("Address not mapped in VMA List\n");
		while(1);
	}
}

short cowBitSet(uint64_t addr){

	//check if the 11th, 0th, 2nd bits are set or not
	if(((0x0000000000000fff) & addr)==0x805){
		return 1;
	}
		return 0;
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
