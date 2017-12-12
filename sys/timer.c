#include<sys/defs.h>
#include<sys/interrupts.h>
#include<sys/kprintf.h>
#include<sys/timer.h>
/*
int timer_ticks = 0;
volatile int currentTime=0;

int bottomColour = 7;
*/

char *convert2(unsigned long num, int base)
{
	static char r[]= "0123456789ABCDEF";
	static char buffer[50];
	char *ptr;

	ptr = &buffer[49];
	*ptr = '\0';

	volatile char * timePtr  = (char*) (0xffffffff800b8000+25*160)-2 ;
	do
	{
		*--ptr = r[num%base];
		*timePtr-- = *ptr;
		*timePtr-- = bottomColour;

		num /= base;
	}while(num != 0);
	return(ptr);
}

void tick_timer(struct isr_regs *r)
{	
	timer_ticks++;

	if((timer_ticks % 100) == 0) {
		currentTime++;
		convert2(currentTime,10);
	}

}
void install_timer()
{

	timer_ticks = 0;
	currentTime=0;

	bottomColour = 7;

	uint16_t TIMER_frequency = 1193180/100;

	//Set the command byte
	outb(0x43,0x36);
	//Set low byte of timer divisor
	outb(0x40, (uint16_t)(TIMER_frequency & 0xFF));
	//Set high byte of timer divisor
	outb(0x40, (uint16_t)(TIMER_frequency >> 8));

}
