#ifndef _PAGEFAULTHANDLER_H_
#define _PAGEFAULTHANDLER_H_

#include<sys/defs.h>
#include<sys/kprintf.h>
#include<sys/cs.h>
#include<sys/processLoader.h>

void handlePageFault();
uint64_t getFaultingAddress();
short isValidAddress(uint64_t faultingAddress);
void mapVirtualAddressToPhysical(uint64_t faultingAddress);
short cowBitSet(uint64_t addr);
#endif
