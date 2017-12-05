#include <sys/defs.h>
#include <sys/interrupts.h>
#include <sys/string.h>
#include <sys/kprintf.h>
#include <sys/pageFaultHandler.h>

#define KERNEL_STACK_SIZE 4096

extern void timer_int();
extern void keyboard_int();
extern void pageFault_int();
/*
 * Set an entry in the IDT
 */
int shiftFlag=0;
int cntlFlag=0;

volatile char *glyphPtr  = (char*) ((0xffffffff800b8000+25*160)-50) ;
volatile char *cntlPtr  = (char*) ((0xffffffff800b8000+25*160)-52) ;

unsigned char kbdus_Shift[128] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
  '(', ')', '_', '+', '\b',	/* Backspace */
  '\t',			/* Tab */
  'Q', 'W', 'E', 'R',	/* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
 '"', '~',   0,		/* Left shift */
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
  'M','<', '>', '?',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};
unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

void pageFaultHandler (){
	kprintf("You are in Page Fault Handler\n");
	handlePageFault();	
}

void idt_set_gate(unsigned long num, unsigned long base, unsigned short select, unsigned char flags)
{
//	kprintf("In setGate\n");
	idt[num].offset_1 = (base) & 0xFFFF;
        idt[num].selector = select;
        idt[num].ist = 0;
        idt[num].type_attr = flags;
        idt[num].offset_2 = (base >> 16) & 0xFFFF;
        idt[num].offset_3 = (base >> 32) & 0xFFFFFFFF;
	idt[num].zero=0;
}

/* Installs the IDT */

void idt_install()
{
//	kprintf("In IDT install\n");
	// Set up the IDT pointer 
	idtp.limit = (sizeof (struct IDTDescr) * 256) - 1;
	idtp.base = (uint64_t) &idt;

	// Initialize IDT table entries to zero 
	//my_memset(&idt, 0, sizeof(struct IDTDescr) * 256);
	_x86_64_asm_lidt(&idtp);
 	init_isr();
}

void init_isr()
{
//	kprintf("In initISR\n");
	idt_set_gate(14, ((uint64_t)pageFault_int),0x08, 0x8E);
	idt_set_gate(32, ((uint64_t)timer_int),0x08, 0x8E);
	idt_set_gate(33, ((uint64_t)keyboard_int),0x08, 0x8E);
}

static void execute_interrupt(struct isr_regs *reg)
{
	//	kprintf("In execute_interrupt\n");
	/*
	 * handle the timer interrupt
	 */
	tick_timer(reg);

	outb(0x20, 0x20);
//		kprintf("Done with timer\n");
}

void interrupt_handler(struct isr_regs *reg)
{//	kprintf("In Interrupt_Handler\n");
	execute_interrupt(reg);
}

void kbInt_handler(struct isr_regs *reg)
{
//	kprintf("In Keyboard Interrupt_Handler\n");
	unsigned char scancode;

	scancode = inb(0x60);

	if (!(scancode & 0x80))
	{	
//		kprintf("Key pressed\n");
		//Checking for Shift key
		if((scancode==42)||(scancode==54)){
			shiftFlag=1; 
			outb(0x20, 0x20);
			return;
		}
		//Checking for control key 
		if((scancode==29)){
			cntlFlag=1;
			outb(0x20, 0x20);
			return;
		}
		if(cntlFlag==1){
			*cntlPtr= '^';
		}else {
			*cntlPtr=' ';
		}
		if(shiftFlag==0){
			//	kprintf("%c",kbdus[scancode]);
			*glyphPtr = kbdus[scancode];
		}else{
			//	kprintf("%c",kbdus_Shift[scancode]);}
			*glyphPtr = kbdus_Shift[scancode];
	}
	*(glyphPtr+1)=7;
	}else
	{	
		if((scancode==170)||(scancode==182)){
			shiftFlag=0;
		}
		if(scancode==157){
			cntlFlag=0;
		}
//		kprintf("Key released\n");
	}
	outb(0x20, 0x20);
//	kprintf("Done with Keyboard Interrupt\n");
}
