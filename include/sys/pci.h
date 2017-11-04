#ifndef _PCI_H
#define _PCI_H
#include<sys/defs.h>
void checkAllBuses();
uint16_t pciConfigReadWord (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t getVendorID(uint8_t bus, uint8_t slot, uint8_t function);

#endif
