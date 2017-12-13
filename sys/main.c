#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
//#include <sys/glDT.h>
#include <sys/interrupts.h>
#include <sys/pic.h>
#include <sys/memoryallocator.h>
//#include <sys/memset_tmp.h>
#include <sys/cs.h>
#include <sys/syscalls.h>
#include <sys/tarfsOps.h>
#include <sys/elfParser.h>
#include <sys/scheduler.h>

#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;
void start(uint32_t *modulep, void *physbase, void *physfree)
{
	pcbCount=0;
	videoMem=0xb8000;
	uint64_t totalMemory=0;
	uint64_t startIndices[10],endIndices[10];
	uint64_t counter=0;
	struct smap_t {
		uint64_t base, length;
		uint32_t type;
	}__attribute__((packed)) *smap;
//	kprintf("physbase  %p\n",(uint64_t)physbase);
	while(modulep[0] != 0x9001) modulep += modulep[1]+2;
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
		if (smap->type == 1 /* memory */ && smap->length != 0) {
//			kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
			
			startIndices[counter] = smap -> base;
			if(startIndices[counter]%0x1000!=0){
				startIndices[counter]=startIndices[counter]>>12;
				startIndices[counter]=startIndices[counter]<<12;
				startIndices[counter]+= 0x1000;
			}
			endIndices  [counter] = smap -> base + smap -> length;
//			kprintf("Range: %p-%p\n",startIndices[counter],endIndices[counter]);
			counter++;
		}
	}
	

	totalMemory = endIndices[counter-1]-startIndices[0];

	initializeMemory(totalMemory,startIndices,endIndices,physbase,physfree,counter,(uint64_t)&kernmem);
//	uint64_t tarFsHdr, elfHdr;
//	char c[11] = {'b','i','n','/','s','b','u','s','h'};	

	//Adding TSS for handling page faults:
	uint64_t pfh_stack[4096];
  	set_tss_rsp((void *)&pfh_stack[4080]);

	//Initialize PCB list
	initializePCBList();
	
	idt_install();
        PIC_remap(0x20,0x20);
        enableInterrupt();
        install_timer();

//	uint64_t* faultyPtr = (uint64_t*) 0x609877;
//	*faultyPtr = 45;
	char c[11] = {'b','i','n','/','h','e','l','l','o'};
	setupMSRs();
	
	//clean code
	loadNewProcess(c);

	//working code
//----------------------------------------------
//	tarFsHdr = searchTarfs(c);
//	elfHdr = tarFsHdr+512;
//	parseElf(elfHdr);
//----------------------------------------------
//	contextSwitch();


//	kprintf("Total memory (int) %d\n",totalMemory);
//	kprintf("physfree %p\n", (uint64_t)physfree);
//	kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
//	kprintf("Kernmem %x\n",(uint64_t)&kernmem);
//	mapKernel((uint64_t) &kernmem);
//	checkAllBuses();
	//port_rebase(abar -> ports, _port);
	//if(i == 1){
        /*int b = write(&abar ->ports[1], startl, starth, 100, buf);
        kprintf("b: %d\n",b);
        if(b == 1){
                 int a = read(&abar ->ports[1], startl, starth, 100, buf2);
                 kprintf("a: %d\n",a);
                 if(a == 1)
                 kprintf("\n%c", buf2);
                 }*/
//        kprintf("Paging done\n");
 //       kprintf("Paging done\n");
   //     kprintf("Paging done\n");	
	while(1);
}

void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
	register char  *temp1,*temp2;
	for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7 /* white */;
	__asm__(
			"cli;"
			"movq %%rsp, %0;"
			"movq %1, %%rsp;"
			:"=g"(loader_stack)
			:"r"(&initial_stack[INITIAL_STACK_SIZE])
	       );
	init_gdt();	
	//idt_install();
	//checkAllBuses();
	//PIC_remap(0x20,0x20);
	//enableInterrupt();
	//install_timer();
	start(
			(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
			(uint64_t*)&physbase,
			(uint64_t*)(uint64_t)loader_stack[4]
	     );
	//	__asm__("int $32;\n");
	//	__asm__("int $32;\n");
	for(
			temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
			*temp1;
			temp1 += 1, temp2 += 2
	   ) *temp2 = *temp1;
	while(1);
}
