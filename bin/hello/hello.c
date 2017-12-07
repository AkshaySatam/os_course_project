#include<stdio.h>
#include<sys/defs.h>

void copyString(char* dest,const char* src, int start, int end){
        while(start < end){
                *dest = *(src+start);
                dest++;
                start++;
        }

        *(dest+start)='\0';
}

long my_fork(){
//      printf("Calling myfork");
        //  unsigned long syscall_write = 1;
	//TODO changing the syscall fork to 2 instead of original 57
        unsigned long syscall_fork = 2;//char* message  = "Hello world\n";
        long ret;
        __asm__ volatile (
                        "movq %1, %%rax\n"
                        "syscall\n"
                        "movq %%rax, %0"
                        : // output parameters, we aren't outputting anything, no none
                        "=r" (ret)
                        : // input parameters mapped to %0 and %1, repsectively
                        "r" (syscall_fork)
                        : // registers that we are "clobbering", unneeded since we are calling exit
                        "rax");
        //      printf("%s\n",buf);
        //              printf("Done\n");
        return ret;
}


int writeB (long fd,char buf[],long length ){
        uint64_t syscall_write = 1;
        uint64_t returnCode=0;
        char buffer[60];
        copyString(buffer,buf,0,length);
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

int main(int argc, char* argv[]){
/*	int a = 1;
	int b= 2;
	int c = 3;
	char* p = (char *)0xffffffff800b8000;

	for(int i=0;i<20;i=i+2)
		*(p+i) = i;

	if(a == 1 && b == 2 && c==3){
		int d = 4;
		if(d){}
	}
*/	
	char buf[30] =  {"Shree Ganesh"};
        writeB(1,buf,12);
	char buf2[30] ={"Om Namah Shivay"};	
        writeB(1,buf2,12);	
	char buf3[30] ={"Keep calm and believe"};	
        writeB(1,buf3,12);	
	//my_fork();
        writeB(1,buf,12);
	while(1){

	}
}



