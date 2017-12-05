#ifndef memoryallocator_h
#define memoryallocator_h
#include<sys/defs.h>
#include<sys/kprintf.h>
#include<sys/cs.h>
struct page_descriptor{
	struct page_descriptor* next;
	struct page_descriptor* nextFree;
	uint32_t startAddress;
	uint32_t endAddress;
	short isfree,isAvailable;
};
uint64_t pgDescCount;
uint64_t pml4;
uint64_t pdpe;
uint64_t pdpePtr;
uint64_t pde;
uint64_t pte;
uint64_t pml4Ptr;
int freeFlag;
uint64_t pbPtr,kernmemPtr;
//uint64_t videoMem;
struct page_descriptor pD;
struct page_descriptor* pgDesc;
struct page_descriptor* pgDescPrev;
struct page_descriptor* freeListHead;
struct page_descriptor* firstFreeLocation;

uint64_t getPML4address(uint64_t p);
uint64_t getPDPEaddress(uint64_t p);
uint64_t getPDEaddress(uint64_t p);
uint64_t getPTEaddress(uint64_t p);

int my_ceil(int num);
uint64_t* kmalloc (uint64_t memorySize);
uint64_t getFreePages(int count);

void mapVirtualAddress(uint64_t faultingAddress);
void excludeUnavailableMemory(uint64_t physfree);
void initializeMemory (uint64_t totalMemory, uint64_t * startIndices, uint64_t*  endIndices, void * physbase, void * physfree, uint64_t counter,uint64_t kernmem);
uint64_t getFreePage();
void pagingSetup();
//void mapEntireMemory(uint64_t startAddress,uint64_t newPhysfree,uint64_t physfree);
void mapEntireMemory(uint64_t newPhysfree);
void mapKernel(uint64_t c,uint64_t physbase,uint64_t newPhysfree);
uint64_t getPML4address(uint64_t c);
void mapVideoCard(uint64_t videoMemory);
void enablePaging();
void remapNextPointers(uint64_t physbase,uint64_t kernmem,uint64_t physfree,uint64_t newphysfree);
void setVirtualAddressForPhysicalMemory(uint64_t pmloff,uint64_t pdpeoff,uint64_t pdeoff,uint64_t pteoff);
#endif
