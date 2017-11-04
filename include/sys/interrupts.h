#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include <sys/defs.h>

#define PF_P	0x1
#define PF_W	0x2
#define PF_U	0x4

struct isr_regs	{
    	uint64_t r9;
	uint64_t r8;

	uint64_t rsi;
	uint64_t rbp;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;
	uint64_t rdi;

	uint64_t int_no;
	uint64_t err_code;

	uint64_t rip;
    	uint64_t cs;
    	uint64_t rflags;
    	uint64_t rsp;
    	uint64_t ss;
}__attribute__((packed));

/* Defines an IDT entry */
struct IDTDescr {
   uint16_t offset_1; // offset bits 0..15
   uint16_t selector; // a code segment selector in GDT or LDT
   uint8_t ist;       // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   uint8_t type_attr; // type and attributes
   uint16_t offset_2; // offset bits 16..31
   uint32_t offset_3; // offset bits 32..63
   uint32_t zero;     // reserved
};

struct idt_ptr
{
    unsigned short limit;
    uint64_t base;
} __attribute__((packed));

struct IDTDescr idt[256];
struct idt_ptr idtp;

extern void _x86_64_asm_lidt(struct idt_ptr* );

/* IO.C */
void outb(uint16_t port, uint32_t val);
uint32_t inb(uint16_t port);
void io_wait();

/* ISR.C */
extern void idt_set_gate(unsigned long num, unsigned long base, unsigned short sel, unsigned char flags);
extern void idt_install();
extern void init_isr();
extern void reinit_PIC();
extern void interrupt_handler(struct isr_regs *);
extern void kbInt_handler(struct isr_regs *);

/* TIMER.C */
extern void install_timer();
extern void tick_timer(struct isr_regs*);

/*interrupt.c*/
extern void enableInterrupt();

/*ahci.c*/
uint64_t checkAllBuses(void);
unsigned long ReadWord (unsigned short bus, unsigned short slot,unsigned short func, unsigned short offset);
unsigned short pciConfigReadWord (unsigned short bus, unsigned short slot,unsigned short func, unsigned short offset);


#endif
