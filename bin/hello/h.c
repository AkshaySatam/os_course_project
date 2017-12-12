#include<stdio.h>
#include<sys/defs.h>



int sleep2(char* time);

int sleep2(char* time){

        //TODO Will change the sys_cal number later
        uint64_t syscall_sleep = 3;
       uint64_t returnCode=0;
        uint64_t buf_addr = (uint64_t)time;

         __asm__ volatile (
                        "movq %1, %%rax\n"
                        "movq %2, %%rdi\n"
                        "syscall\n"
                        "movq %%rax, %0\n"
                        : // output parameters, we aren't outputting anything, no none
                        "=m" (returnCode)
                        : // input parameters mapped to %0 and %1, repsectively
                        "r" (syscall_sleep), "r"(buf_addr)
                        : // registers that we are "clobbering", unneeded since we are calling exit
                        "rax", "rdi");
        return returnCode;
}


int main(int argc, char* argv[],char* envp[]){
	while(1){
		sleep2("5000");
	}
}

