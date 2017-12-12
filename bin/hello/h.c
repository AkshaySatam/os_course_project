#include<stdio.h>
#include<sys/defs.h>



void sleep2(char* time);

void sleep2(char* time){

        //TODO Will change the sys_cal number later
        uint64_t syscall_sleep = 3;
       // uint64_t returnCode=0;
        uint64_t buf_addr = (uint64_t)time;
        //kprintf("data is %s\n", buffer);

         __asm__ volatile (
                        "movq %0, %%rax\n"
                        "movq %1, %%rdi\n"
                        "syscall\n"
        //                "movq %%rax, %0\n"
                        : // output parameters, we aren't outputting anything, no none
                       // "=r" (returnCode)
                        : // input parameters mapped to %0 and %1, repsectively
                        "r" (syscall_sleep), "r"(buf_addr)
                        //, "m" (syscall_exit),"m" (exit_status)
                        : // registers that we are "clobbering", unneeded since we are calling exit
                        "rax", "rdi");
      //  return returnCode;
}


int main(int argc, char* argv[],char* envp[]){
	while(1){
		sleep2("1000");
	}
}

