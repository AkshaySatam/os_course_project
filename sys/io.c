#include <sys/interrupts.h>
#include <sys/defs.h>

void outb(uint16_t port, uint32_t val){
	__asm__ volatile ("outb %%al, %%dx"::"d" (port), "a" (val));

}

uint32_t inb(uint16_t port) {
	uint32_t ret;

	__asm__ volatile ("inb %%dx,%%al":"=a"(ret):"d"(port));

	return ret;
}

void io_wait(){
        __asm volatile ( "outb %%al, $0x80" : : "a"(0) );
           
}
