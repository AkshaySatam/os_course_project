#include<stdio.h>
#include<sys/defs.h>


void readInput(char* inputBuffer);
int write (long fd,char buf[],long length );
void displayPrompt();
void copyString(char* dest,const char* src, int start, int end);
int read (long fd,char buf[],long length);
int stringLength(const char* str);


char buffer[10][256];
char *bufptr[10];
char* prompt;

void displayPrompt()
{
        write(1,prompt,1);
}


void copyString(char* dest,const char* src, int start, int end){
        while(start < end){
                *dest = *(src+start);
                dest++;
                start++;
        }

        *(dest+start)='\0';
}


int write (long fd,char buf[],long length ){
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

int read (long fd,char buf[],long length){
        uint64_t syscall_read = 0;
        uint64_t returnCode=0;
        char buffer[60];
        copyString(buffer,buf,0,length);
        uint64_t  buf_addr = (uint64_t) buffer;

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
                        "r" (syscall_read), "r" (fd), "r" (buf_addr),"r" (length)
                        //, "m" (syscall_exit),"m" (exit_status)
                        : // registers that we are "clobbering", unneeded since we are calling exit
                        "rax", "rdi","rsi","rdx");
        return returnCode;
}

int main(int argc, char* argv[]){
	prompt = "$";

        char  inputBuffer[20];
      /*  char* splittedInput[10];

        int bufc;
        for(bufc = 0; bufc < 10; bufc++)
        {
                splittedInput[bufc] = &buffer[bufc][0];
                bufptr[bufc] = &buffer[bufc][0];
        }
	*/
        //char spaceDelimiter = ' ';
        while(1)
        {
                displayPrompt();
		readInput(inputBuffer);
		write(1,inputBuffer,stringLength(inputBuffer));
	}

}

int stringLength(const char* str){
        int i=0;
        int count=0;
        while(*(str+i)!='\0'){
                count++;
                i++;
        }
        return count;
}

void readInput(char* inputBuffer){
        int j=0,i=0;
        char  buffer[512];
        read(0,buffer,512);
        while(buffer[j]!='\n')
        {
                inputBuffer[i++]=buffer[j++];
        }
        inputBuffer[i]='\0';
}

/*
	char buf[30] =  {"Shree Ganesh"};
        write(1,buf,12);
	char buf2[30] ={"Om Namah Shivay"};	
        write(1,buf2,12);	
	char buf3[30] ={"Keep calm and believe"};	
        write(1,buf3,12);	
	my_fork();
        read(1,buf,12);
	char buf4[30] ={"Yes... you can"};	

        writeB(1,buf4,12);	
	while(1){

	}

*/
