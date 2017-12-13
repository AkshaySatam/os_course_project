#include<stdio.h>
#include<sys/defs.h>



int sleep2(char* time);
int open2(char* path, uint64_t flags);

void copyString2(char* dest,const char* src, int start, int end);

char inputBuffer[256];

void copyString2(char* dest,const char* src, int start, int end){
        while(start < end){
                *dest = *(src+start);
                dest++;
                start++;
        }

        *(dest+start)='\0';
}

int open2(char* path, uint64_t flags){
	//TODO Will change the sys_cal number later
	uint64_t syscall_open = 4;
	uint64_t fd;
	uint64_t buf_addr = (uint64_t)path;

	__asm__ volatile (
			"movq %1, %%rax\n"
			"movq %2, %%rdi\n"
			"movq %3, %%rsi\n"
			"syscall\n"
			"movq %%rax, %0\n"
			: // output parameters, we aren't outputting anything, no none
			"=m" (fd)
			: // input parameters mapped to %0 and %1, repsectively
			"r" (syscall_open), "r"(buf_addr), "r"(flags)
			: // registers that we are "clobbering", unneeded since we are calling exit
			"rax", "rdi", "rsi");
	return fd;
}

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

int write2(long fd,char buf[],long length ){
        uint64_t syscall_write = 1;
        uint64_t returnCode=0;
        char buffer[60];
        copyString2(buffer,buf,0,length);
        uint64_t  buf_addr = (uint64_t) buffer;
//      kprintf("data is %s\n", buffer);

         __asm__ volatile (
                        "movq %1, %%rax\n"
                        "movq %2, %%rdi\n"
                        "movq %3, %%rsi\n"
                        "movq %4, %%rdx\n"
                        "syscall\n"
                        "movq %%rax, %0\n"
                        : // output parameters, we aren't outputting anything, no none
                        "=r" (returnCode)
                        : // input parameters mapped to %0 and %1, repsectively
                        "r" (syscall_write), "r" (fd), "r" (buf_addr),"r" (length)
                        //, "m" (syscall_exit),"m" (exit_status)
                        : // registers that we are "clobbering", unneeded since we are calling exit
                        "rax", "rdi","rsi","rdx");
        return returnCode;
}

int read2(long input_file_desc,char buf[],long buf_size){

        unsigned long syscall_read = 0;
        long buf_addr = (long)inputBuffer;
        long ret;
        __asm__ __volatile__ (
                        "movq %1, %%rax\n"
                        "movq %2, %%rdi\n"
                        "movq %3, %%rsi\n"
                        "movq %4, %%rdx\n"
                        "syscall\n"
                        "movq %%rax, %0"
                        : /* output parameters, we aren't outputting anything, no none */
                        "=m" (ret)
                        : /* input parameters mapped to %0 and %1, repsectively */
                        "m" (syscall_read),"m"(input_file_desc),"m"(buf_addr), "m"(buf_size)
                        : /* registers that we are "clobbering", unneeded since we are calling exit */
                        "rax", "rdi","rdx","rsi");
        return ret;
}

int main(int argc, char* argv[],char* envp[]){

	int fd = open2("etc/dummy.txt",1234);
	read2(fd,inputBuffer,20);
//	write2(1,inputBuffer,20);
	while(1){
		sleep2("5000");
	}
}
