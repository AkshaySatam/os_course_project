#define _GNU_SOURCE
//#include <stdio.h>
#include <unistd.h>
//#include "wait.c"
//#include <errno.h>
//#include <string.h>
//#include <stdlib.h>
//#include "chdir.c"
//#include "stringUtils.c"
#define PROT_READ	0x1     /* Page can be read.  */
#define PROT_WRITE	0x2     /* Page can be written.  */
#define PROT_EXEC	0x4     /* Page can be executed.  */
#define MAP_ANONYMOUS	0x20    /* Don't use a file.  */

void concatenateString(char dest[], const char* b,const char* c);
int  compareStrings(const char* a,const char* b);
void splitString(char* buffer [10],const char* input,char delimiter);
int stringLength(const char* str);
void copyString(char* dest,const char* src, int start, int end);

char* prompt;

void *mmap(void *addr, unsigned long length, unsigned long prot, unsigned long flags,
		long fd, long offset){
	length = 4096;
	unsigned long syscall_mmap = 9;
	unsigned long a = (unsigned long)addr;
	unsigned long memPtr;
	__asm__ (
			"movq %1, %%rax\n"
			"movq %2, %%rdi\n"
			"movq %3, %%rsi\n"
			"movq %4, %%rdx\n"
			"movq %5, %%r10\n"
			"movq %6, %%r8\n"
			"movq %7, %%r9\n"
			"syscall\n"
			"movq %%rax, %0\n"
			:
			"=r" (memPtr)
			:
			"r" (syscall_mmap), "r" (a), "r" (length),"r" (prot), "r" (flags),"r" (fd),"r" (offset)
			: // registers that we are "clobbering", unneeded since we are calling exit
			"rax", "rdi","rsi","rdx","r10","r8","r9");

	return (void*)memPtr;
}

void* malloc(long size){
	return mmap(NULL, size, PROT_READ|PROT_WRITE|PROT_EXEC,
			MAP_ANONYMOUS, -1, 0);
}

void print(char* message){

  unsigned long syscall_write = 1;
  unsigned long syscall_exit = 60;
  long file_desc=1;
  long  no_bytes=stringLength(message);
  long  exit_status = 42;
  __asm__ (
        "movq %0, %%rax\n"
        "movq %1, %%rdi\n"
        "movq %2, %%rsi\n"
        "movq %3, %%rdx\n"
        "syscall\n"
//      "movq %4, %%rax\n"
//      "xor %5,  %%rdi\n"
//      "syscall"
    : // output parameters, we aren't outputting anything, no none
    : // input parameters mapped to %0 and %1, repsectively
      "m" (syscall_write), "m" (file_desc), "m" (message),"m" (no_bytes),"m" (syscall_exit), "m" (exit_status)
    : // registers that we are "clobbering", unneeded since we are calling exit 
      "rax", "rdi","rsi","rdx");
}

int my_write (long output_file_desc,char* string)
{

        long length = (long) stringLength(string);
        char buffer[length];
        copyString(buffer,string,0,length);
        long syscall_write = 1;
        long buf_addr = (long) buffer;
        long returnCode;
       __asm__ (
                        "movq %1, %%rax\n"
                        "movq %2, %%rdi\n"
                        "movq %3, %%rsi\n"
                        "movq %4, %%rdx\n"
                        "syscall\n"
                        "movq %%rax, %0\n"
                        : // output parameters, we aren't outputting anything, no none
                        "=r" (returnCode)
                        : // input parameters mapped to %0 and %1, repsectively
                        "r" (syscall_write), "r" (output_file_desc), "r" (buf_addr),"r" (length)
                        //, "m" (syscall_exit),"m" (exit_status)
                        : // registers that we are "clobbering", unneeded since we are calling exit
                        "rax", "rdi","rsi","rdx");
        //printf("The end");
        return returnCode;
}


int my_read(long input_file_desc,char buf[],long buf_size){

  unsigned long syscall_read = 0;
  long buf_addr = (long)buf;
  long ret;
  __asm__ (
        "movq %1, %%rax\n"
        "movq %2, %%rdi\n"
        "movq %3, %%rsi\n"
        "movq %4, %%rdx\n"
        "syscall\n"
        "movq %%rax, %0"
    : // output parameters, we aren't outputting anything, no none 
     "=m" (ret)
    : // input parameters mapped to %0 and %1, repsectively 
      "m" (syscall_read),"m"(input_file_desc),"m"(buf_addr), "m"(buf_size)
    : // registers that we are "clobbering", unneeded since we are calling exit 
      "rax", "rdi","rdx","rsi");
//printf("%d\n",ret);
//printf("%s\n",buf);
//printf("Done\n");
return ret;
}


int  my_chdir(const char* fileName){

        unsigned long fName  = (long) fileName;
        unsigned long syscall_chdir = 80;
        unsigned long syscall_getCwd = 79;
        unsigned int returnCode;
        unsigned long buf_size = 100;
        char buffer[buf_size];
        long buf = (long) buffer;
        char  returnString [100];
        __asm__ (
                        "movq %1, %%rax\n"
                        "movq %2, %%rdi\n"
                        "syscall\n"
                        "movq %%rax, %0\n"
                        : // output parameters, we aren't outputting anything, no none
                        "=m" (returnCode)
                        : // input parameters mapped to %0 and %1, repsectively
                        "m" (syscall_chdir), "m" (fName)
                        : // registers that we are "clobbering", unneeded since we are calling exit 
                        "rax","rdi");
//      printf("Output from chdir %d \n",returnCode);

        __asm__ (
                        "movq %1, %%rax\n"
                        "movq %2, %%rdi\n"
                        "movq %3, %%rsi\n"
                        "syscall\n"
                        "movq %%rax, %0\n"
                        : // output parameters, we aren't outputting anything, no none
                        "=m" (returnString)
                        : // input parameters mapped to %0 and %1, repsectively
                        "m" (syscall_getCwd), "m" (buf),"m" (buf_size)
                        : // registers that we are "clobbering", unneeded since we are calling exit
                        "rax","rdi","rsi");
//      printf("After returning from getCwd\n");
//      printf("Output from getCwd  %s \n",returnString[0]);
        my_write(1,"Current Working Directory is : ");
        my_write(1,buffer);
//      printf("Err no %d\n",errno);
        return returnCode;
}

void my_wait(){
        unsigned long syscall_wait =61 ;
        long options=0,waitStatus;
        struct rusage *rusage;
        int* wstatus;
        long ret=0;
        __asm__ (
                        "movq %1, %%rax\n"
                        "movq %2, %%rdi\n"
                        "movq %3, %%rsi\n"
                        "movq %4, %%rdx\n"
                        //      "movq %5, %%rdx\n"
                        "syscall\n"
                        "movq %%rax, %0\n"
                        : // output parameters, we aren't outputting anything, no none
                        "=m" (waitStatus)
                        :// input parameters mapped to %0 and %1, repsectively
                        "m" (syscall_wait),"m" (ret),"m" (wstatus),"m" (options),"m" (rusage)
                        : // registers that we are "clobbering", unneeded since we are calling exit
                        "rax","rdi","rsi","rdx");
}
long my_fork(){
//      printf("Calling myfork");
        //  unsigned long syscall_write = 1;
        unsigned long syscall_fork = 57;//char* message  = "Hello world\n";
        long ret;
        __asm__ (
                        "movq %1, %%rax\n"
                        "syscall\n"
                        "movq %%rax, %0"
                        : // output parameters, we aren't outputting anything, no none
                        "=m" (ret)
                        : // input parameters mapped to %0 and %1, repsectively
                        "m" (syscall_fork)
                        : // registers that we are "clobbering", unneeded since we are calling exit
                        "rax");
        //      printf("%s\n",buf);
        //              printf("Done\n");
        return ret;
}

void my_dup2(unsigned int newFd, unsigned int oldFd){

        unsigned long syscall_dup2 = 33;
//      unsigned int oldFd=1;
        unsigned int returnCode;
//      unsigned int newFd = open("./text.txt", O_WRONLY | O_CREAT,0644);
        __asm__ (
                        "movq %1, %%rax\n"
                        "movq %2, %%rdi\n"
                        "movq %3, %%rsi\n"
                        "syscall\n"
                        "movq %%rax, %0\n"
                        : // output parameters, we aren't outputting anything, no none
                        "=m" (returnCode)
                        : // input parameters mapped to %0 and %1, repsectively
                        "m" (syscall_dup2), "m" (newFd), "m" (oldFd)
                        : // registers that we are "clobbering", unneeded since we are calling exit
                        "rax","rdi","rsi");
//        printf("Output from dup2 %d \n",returnCode);
}
long my_execvpe(char* fileName,char* arg[],char* envp[]){

        unsigned long syscall_execve =59 ;
        long returnCode;
//      const char* fullFileNamePointer = concatenateString("/bin/",fileName);
        //const char* fileName = "cat";
        //char* fullFileName[40];
        int length =stringLength(fileName)+5;
        char fullFileName[length];
        concatenateString(fullFileName,"/bin/",fileName);
        copyString(arg[0],fullFileName,0,length);
//      printf("arg[0] %s\n",arg[0]);
//      printf("Full file name: %s \n",fullFileName);
        unsigned long fName = (long) fullFileName;
        //char* arg [5] = {"cat",NULL};
        //char* envp [5] = {NULL};
        unsigned long arg_addr  = (long) arg;
        unsigned long envp_addr  = (long) envp;
        //printf("Starting with cat\n");
        __asm__ (
                        "movq %1, %%rax\n"
                        "movq %2, %%rdi\n"
                        "movq %3, %%rsi\n"
                        "movq %4, %%rdx\n"
                        "syscall\n"
                        "movq %%rax, %0\n"
                        : // output parameters, we aren't outputting anything, no none
                        "=m" (returnCode)
                        : // input parameters mapped to %0 and %1, repsectively
                        "r" (syscall_execve),"r"(fName),"r"(arg_addr), "r"(envp_addr)
                        : // registers that we are "clobbering", unneeded since we are calling exit
                        "rax", "rdi","rdx","rsi");
        //printf("Return code %u \n",returnCode);
        //printf("Out of exec.");
       // printf("Opened File");
        return returnCode;
}

void my_pipe(int pipeFd[2]){
        unsigned long syscall_pipe = 22;
        long returnCode;
        __asm__ (
                        "movq %1, %%rax\n"
                        "movq %2, %%rdi\n"
                        "syscall\n"
                        "movq %%rax, %0\n"
                        : // output parameters, we aren't outputting anything, no none
                        "=m" (returnCode)
                        : // input parameters mapped to %0 and %1, repsectively
                        "m" (syscall_pipe), "m" (pipeFd)
                        : // registers that we are "clobbering", unneeded since we are calling exit
                        "rax","rdi");

        //printf("Output from pipe command  %d \n",returnCode);
}


void my_close(unsigned int oldFd){
        unsigned long syscall_close = 3;
        unsigned int returnCode;
        __asm__ (
                        "movq %1, %%rax\n"
                        "movq %2, %%rdi\n"
                        "syscall\n"
                        "movq %%rax, %0\n"
                        : // output parameters, we aren't outputting anything, no none
                        "=m" (returnCode)
                        : // input parameters mapped to %0 and %1, repsectively
                        "m" (syscall_close), "m" (oldFd)
                        : // registers that we are "clobbering", unneeded since we are calling exit
                        "rax","rdi");
        //printf("Return code after close: = %d",returnCode);
}


/*char* concatenateString(char* a, char* b){
	char *dest = (char*)malloc(sizeof(char)*(stringLength(a)+stringLength(b)+1));
	int i=0,j=0;
	while(*(a+i)!='\0'){
		*(dest+i)=*(a+i);
		i++;
	}
	
	while(*(b+j)!='\0'){
		*(dest+i+j)=*(b+j);
		j++;
	}
	*(dest+i+j)='\0';
	return dest;
}
*/
/*
void print(char* message){

  unsigned long syscall_write = 1;
  unsigned long syscall_exit = 60;
  //char* message  = "Hello world\n";
  long file_desc=1;
  long  no_bytes=stringLength(message);
  long  exit_status = 42;
  __asm__ (	
	"movq %0, %%rax\n"
       	"movq %1, %%rdi\n"
	"movq %2, %%rsi\n"
	"movq %3, %%rdx\n"
       	"syscall\n"
//	"movq %4, %%rax\n"
//	"xor %5,  %%rdi\n"
//	"syscall"
    : // output parameters, we aren't outputting anything, no none
      // (none)
    : // input parameters mapped to %0 and %1, repsectively 
      "m" (syscall_write), "m" (file_desc), "m" (message),"m" (no_bytes),"m" (syscall_exit), "m" (exit_status)
    : // registers that we are "clobbering", unneeded since we are calling exit
      "rax", "rdi","rsi","rdx");
}
*/
int  compareStrings(const char* a,const char* b){
	//if(stringLength(a)==stringLength(b)){
		int i=0;
		while(*(a+i)!='\0'){
			if(*(a+i)!=*(b+i)){
				return 0;
			}
			i++;
		}
		return 1;
	//}
	//return false;
}


void splitString(char* buffer [10],const char* input,char delimiter){
	int pos=0, start=0;
	int count=0;
	while((input[pos])!='\0'){
		if(input[pos]==delimiter){
			buffer[count]= (char *) malloc(((pos-start)+1)*sizeof(char));
//			printf("Values of start: %d, pos: %d",start,pos); 
			copyString(buffer[count],input,start,pos);
//			printf("Tokenized string for pipe: %s\n",buffer[count]);
			start = pos+1;
			count++;
		}
		pos++;
//		printf("Value of pos: %d",pos);
	}
//	printf("Values of start: %d, pos: %d",start,pos); 
	buffer[count]= (char *) malloc(((pos-start)+1)*sizeof(char));
	copyString(buffer[count],input,start,pos);
//	printf("Tokenized string for pipe: %s\n",buffer[count]);
//	printf("Tokenized srings: %s %s %s %s ",buffer[0],buffer[1],buffer[2],buffer[3]);
}


void initializeStringArray(char* splittedInput[]){
	int i =0;
	while(i<10){
		splittedInput[i++]="\0";
	}
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

void changeDirectory(char* inputArgs){
//	char cwd[200];
	int i;
	//char* c;
	const char* dir = inputArgs;	
	if((i = my_chdir(dir))!=0){
//		printf("%d",errno);
	}
	else{
	//	c = getcwd(cwd,200);
	//	my_write(1,c);
//		printf("Current working directory %s",cwd);
//		printf("%s",c);
	}  
}

void executeInAnotherProcess(char * argv[])
{
	//printf("\nExecuting ls\n");
	int returnCode = my_fork();
	//int i ;
	if(returnCode<0) {
		//printf("Error");  
	} 
	else if (returnCode==0) {
		//printf("This is child");
		char* envp[]= {0};
		//argv[0]="/bin/sh";
		my_execvpe(argv[0],argv,envp);
		//i = execve(argv[0],argv,envp);
		//printf("Status of execve %d",i);
		//printf("Child exiting");
	}
	else{
		//printf("This is parent");
		my_wait(NULL);
	}
}

void readInput(char* inputBuffer){
	/*char c;
	int i =0;
	while(1){
		c = getchar();
		if(c == '\n'){
			inputBuffer[i]=0; 
			break;
		}
		inputBuffer[i++]=c;
	}*/
	int j=0,i=0;
	char  buffer[512];
	//long readBytes;
	//readBytes = my_read(0,buffer,512);
	my_read(0,buffer,512);
	//printf("Read bytes. going into while loop");
	while(buffer[j]!='\n')
	{
		inputBuffer[i++]=buffer[j++];
	}
	inputBuffer[i]='\0';
	//printf("Input %s\n",inputBuffer);
}
/*
void setEnvironmentVariables(char* splittedInput[10])
{
	char * path ="PATH";

	char* args[10];
	splitString(args,splittedInput[1],'=');

	if(!strcmp(args[0],path))
	{
		setenv(args[1],concatenateString(concatenateString(getenv("PATH"),":"),args[1]),1);
	//	printf("After changing the environment variable is %s\n", getenv("PATH"));
	}
}
*/
void runInBackground(char* argv[10],char* envp[]){
	//printf("Running thread in background");	
	int returnCode = my_fork();
	//Concatenating strings
//	char* str = concatenateString(argv[0],argv[1]);
	if(returnCode<0) {
	//	printf("Error");  
	} 
	else if (returnCode==0) {
	//	printf("This is child");
	//	char* envp[]= {0};
	//	argv[0]=str;
	//	argv[1]=NULL;
		//char * arg[2];
		//arg[0]=ar:
	//	printf("the arguments are: %s %s ",argv[0],argv[1]);
	//	printf("Executing command in child");
		my_execvpe(argv[0],argv,envp);
	//	printf("Status of execve %d",i);
	//	printf("Child exiting");
	}
	else{
	//	printf("This is parent");
	}
}

void processInput(char *splittedInput[]){
	//printf("FIrst word of splitted input");
	//printf("%s\n",splittedInput[0]);
	if(splittedInput[1]!=NULL)
	{
		if(compareStrings("&",splittedInput[1])==1) {
			//runInBackground(splittedInput);
			return;
		}
	} 
	if(compareStrings("cd",splittedInput[0])==1) {
		changeDirectory(splittedInput[1]);
	}
	else if (compareStrings("ls",splittedInput[0])==1){
		//printf("Executing ls in another process");
		executeInAnotherProcess(splittedInput);
	}
	else if(compareStrings("sh",splittedInput[0])==1){
		executeInAnotherProcess(splittedInput);
	}
	else if(compareStrings("export",splittedInput[0])==1){
		//setEnvironmentVariables(splittedInput);
	}
	else {
		//	print("Command not found");
	}
}

void scanInput(char* splittedInput[10],char* input,char delimiter)
{
	splitString(splittedInput,input,delimiter);	
}

void displayPrompt()
{
        //printf("%c",prompt);
        my_write(1,prompt);
}


/*
void freeMemory(char* input[]){
	int i=0;
 	const char* nullCharacter = "\0";
	while(input[i]!=nullCharacter){
		free(input[i++]);
	}
}
*/

int containsPipe(char* input){
	int i=0;
	while(*(input+i)!='\0')
	{
		if(*(input+i)=='|')
		{
			return 1;
		}
		i++;
	}
	return 0;
}

int countPipes(char* input){
	int i=0;
	int count=0;
	while(*(input + i)!='\0'){
		if(*(input+i)=='|'){
			count++;	
	}
	i++;
	}
	return count;
}

void implementPipe(char* input){
	//	printf("In pipe impl\n");
	int count = countPipes(input);
	//	printf("Number of pipes: %d\n",count);
	char* commands [10];
	splitString(commands,input,'|');
	int i=0;

	while(count>0)
	{	
		int pipefd[2];
		my_pipe(pipefd);
		//printf("%d",p);
		//		printf("Command in pipe :%s \n",commands[i]);


		int pid = my_fork();
		if(pid<0){
			//				printf("Error");
		}else if(pid==0){
			char* argv[10]; 
			//				printf("In Child - pipe");
			my_dup2(pipefd[1],1);
			initializeStringArray(argv);
			splitString(argv,commands[i],' ');
			addNullAtTheEnd(argv);
			char* envp[]= {0};
			my_execvpe(argv[0],argv,envp);

		}else{	
			my_wait(NULL);
			//				printf("In parent - pipe");
			my_dup2(pipefd[0],0);
			my_close(pipefd[1]);
		}
		i++;
		count--;

	}
	//	printf("Out of while");
	//	printf("Command:%s ",commands[i]);
	char* argv[10];
	initializeStringArray(argv);
	splitString(argv,commands[i],' ');
	addNullAtTheEnd(argv);
	char* envp[]= {0};
	my_execvpe(argv[0],argv,envp);
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

void testSyscall(){
//      __asm__ volatile("syscall\n");
//      kprintf("Back in user space\n");
	char buf[30] =  {"Shree Ganesh"};
        writeB(1,buf,12);
//      kprintf("Thank you\n");
//      kprintf("Back in user space\n");
     //   buf = "Om Namah Shivay";
      //  writeB(1,buf,20);
       // buf = "Keep calm";
      //  writeB(1,buf,20);
       // buf = "and believe";
      //  writeB(1,buf,12);
	while(1);
}

int main(int argc, char *argv[], char *envp[]) {
	prompt = "$";
	
/*	char  inputBuffer[20];
	char* splittedInput[10];
	char spaceDelimiter = ' ';
	int hasPipe;
	while(1)
	{  	
		displayPrompt();    
		readInput(inputBuffer);
		initializeStringArray(splittedInput);
		hasPipe = containsPipe(inputBuffer);
		if(hasPipe==1){
			implementPipe(inputBuffer);
		}else{
			scanInput(splittedInput,inputBuffer,spaceDelimiter);
			addNullAtTheEnd(splittedInput);
			processInput(splittedInput);
		}
	}
	//freeMemory(splittedInput);
*/	
	char buf[30] =  {"Shree Ganesh"};
        writeB(1,buf,12);
	return 0;
}


int stringLength(const char* str){
//      printf("in Calculating Length\n");
        int i=0;
        int count=0;
//      printf("Original string %s  \n ",str);
        while(*(str+i)!='\0'){
  //              printf("Character at %d is: %c",i,str[i]);
                count++;
                i++;
        }
//      printf("Length: %d \n",count);
        return count;
}


void copyString(char* dest,const char* src, int start, int end){
        while(start < end){
                *dest = *(src+start);
                dest++;
                start++;
        }

        *(dest+start)='\0';
}


void  concatenateString(char dest[],const char* a,const char* b){
        int i=0,j=0;
        while(*(a+i)!='\0'){
                *(dest+i)=*(a+i);
                //printf("%c",dest[i]);
                i++;
        }

        while(*(b+j)!='\0'){
                *(dest+i+j)=*(b+j);
               // printf("%c",dest[j]);
                j++;
        }

        *(dest+i+j)='\0';
}


