#ifndef memoryallocator_h
#define memoryallocator_h
#include<sys/defs.h>

struct page_descriptor{
struct page_descriptor* next;
uint32_t startAddress;
uint32_t endAddress;
short isfree;
};

struct page_descriptor* pgDesc;
struct page_descriptor* freeListHead;
//struct page_descriptor pD;
void initializeMemory (uint64_t totalMemory, uint64_t * startIndices, uint64_t*  endIndices, void * physbase, void * physfree, uint64_t counter);

#endif
