#include<stdio.h>
#include<sys/defs.h>


void readInput();
int write (long fd,char buf[],long length );
void displayPrompt();
void copyString(char* dest,const char* src, int start, int end);
int read (long fd,char buf[],long length);
int stringLength(const char* str);
void scanInput(char* input,char delimiter);
void splitString(const char* input,char delimiter);
void initializeStringArray(char* splittedInput[]);
void processInput(char* envp[]);
void addNullAtTheEnd(char* argv []);
void clearBuffer(char* c);
void cat(char* fileName);
int sleep(char* time);
int open(char* path, uint64_t flags);
int close(uint64_t fd);


char buffer[10][256];
char *bufptr[10];
char* prompt;
char  inputBuffer[5000];

void displayPrompt()
{
        write(1,prompt,1);
}

int open(char* path, uint64_t flags){
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
                        "=r" (fd)
                        : // input parameters mapped to %0 and %1, repsectively
                        "r" (syscall_open), "r"(buf_addr), "r"(flags)
                        : // registers that we are "clobbering", unneeded since we are calling exit
                        "rax", "rdi", "rsi");
        return fd;
}


int close(uint64_t fd){
        //TODO Will change the sys_cal number later
        uint64_t syscall_close = 5;
        uint64_t returnAdd;

        __asm__ volatile (
                        "movq %1, %%rax\n"
                        "movq %2, %%rdi\n"
                        "syscall\n"
                        "movq %%rax, %0\n"
                        : // output parameters, we aren't outputting anything, no none
                        "=r" (returnAdd)
                        : // input parameters mapped to %0 and %1, repsectively
                        "r" (syscall_close), "r"(fd)
                        : // registers that we are "clobbering", unneeded since we are calling exit
                        "rax", "rdi");
        return returnAdd;
}

void copyString(char* dest,const char* src, int start, int end){
	int i =0;
        while(start < end){
                *(dest+i) = *(src+start);
                i++;
                start++;
        }

        *(dest+i)='\0';
}

int write (long fd,char buf[],long length ){
	uint64_t syscall_write = 1;
	uint64_t returnCode=0;
	char buffer[5000];
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

int read(long input_file_desc,char buf[],long buf_size){
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
			"=r" (ret)
			: /* input parameters mapped to %0 and %1, repsectively */
			"m" (syscall_read),"m"(input_file_desc),"m"(buf_addr), "m"(buf_size)
			: /* registers that we are "clobbering", unneeded since we are calling exit */
			"rax", "rdi","rdx","rsi");
	return ret;
}

int main(int argc, char* argv[],char* envp[]){
	prompt = "$";

        int bufc;
        for(bufc = 0; bufc < 10; bufc++)
        {
                bufptr[bufc] = &buffer[bufc][0];
        }
	
        char spaceDelimiter = ' ';
        while(1)
        {
                displayPrompt();
		readInput();
                scanInput(inputBuffer,spaceDelimiter);
                processInput(envp);
	}

}

int compareStrings(const char* a,const char* b){
                int i=0;
                while(*(a+i)!='\0'){
                        if(*(a+i)!=*(b+i)){
                                return 0;
                        }
                        i++;
                }
                return 1;
}

void addNullAtTheEnd(char* argv []){
        int i =0;
        const char* nullCharacter = "\0";
        while(argv[i]!=nullCharacter){
                //printf("Value at position %d is: %s",i,argv[i]);
                i++;
        }
        argv[i]=NULL;
}

void clearBuffer(char* c){
	while(*c!='\0'){
		*c++='\0';
	}
}

int sleep(char* time){
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

void cat(char* fileName){
	int count;
	int fd = open(fileName,1234);
	if(fd!=1){
		count = read(fd,inputBuffer,5000);
		write(1,inputBuffer,count);
		close(fd);
	}
}

void ps(){

}

void processInput(char* envp[]){
	int l;
        if(compareStrings("echo",&buffer[0][0])==1) {
		l = stringLength(&buffer[1][0]);
                write(1,&buffer[1][0],l);
		clearBuffer(&buffer[1][0]);
		clearBuffer(&buffer[0][0]);
		return;
        }
        if(compareStrings("sleep",&buffer[0][0])==1) {
		
		int i =0;
		while(buffer[1][i]!='\n'){
			i++;
		}
		buffer[1][i]='\0';
	
		sleep(&buffer[1][0]);
		clearBuffer(&buffer[1][0]);
		clearBuffer(&buffer[0][0]);
		return;    
   	}
	if(compareStrings("cat",&buffer[0][0])){
		int i =0;
		while(buffer[1][i]!='\n'){
			i++;
		}
		buffer[1][i]='\0';
	
		cat(&buffer[1][0]);
		clearBuffer(&buffer[1][0]);
		clearBuffer(&buffer[0][0]);
		return;	
	}
	if(compareStrings("ps",&buffer[0][0])){
		ps();
		return;	
	}
}


void scanInput(char* input,char delimiter)
{
        splitString(input,delimiter);
}

void splitString(const char* input,char delimiter){
        int pos=0, start=0;
        int count=0;
        while((input[pos])!='\0'){
                if(input[pos]==delimiter){
                        copyString(buffer[count],input,start,pos);
                        start = pos+1;
                        count++;
                }
                pos++;
        }
        copyString(buffer[count++],input,start,pos);
        bufptr[count] = NULL;
}

void initializeStringArray(char* splittedInput[]){
        int i =0;
        while(i<10){
                splittedInput[i++]="\0";
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

void readInput(){
	read(0,inputBuffer,512);
/*	int i =0;
	while(inputBuffer[i]!='\n'){
		i++;
	}
	inputBuffer[i]='\0';*/
}

