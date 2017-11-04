#include<sys/kprintf.h>
#include<sys/stdarg.h>
int find(char* string,char c);
void scrolldown();
char* convert(unsigned int , int); 

volatile char *video = (volatile char*)0xB8000;
int count=0,line=0;
int colour = 7;

void kprintf(const char* format, ...){
	const char *traverse;
	char c; 
	char *s;
	int i;
	long l;
	char finalArray [1000];
	char* traverseFinalArray;
	traverseFinalArray = finalArray;
	va_list arg; 
	va_start(arg, format); 
	traverse = format;
	while( *traverse != '\0') {
		if( *traverse != '%' ) {
			*traverseFinalArray = *traverse;
			traverseFinalArray++; 
		}else{
			traverse++;
			switch(*traverse) 	{ 
				case 's': s = va_arg(arg,char *); 		//Fetch string
					  while(*s!='\0'){
						  *traverseFinalArray++ = *s++;
					  }
					  break;
				case 'c': c = va_arg(arg,int);
					  *traverseFinalArray++ = c;
					  break; 
				case 'd' : i = va_arg(arg,int); 		
						
						char *str = convert(i,10);
						while(*str!='\0'){
                                                  *traverseFinalArray++ = *str++;
                                          }
						break; 			
				case 'x' : i = va_arg(arg,int);
						char *a = "0x";
						while(*a!='\0'){
                                                  *traverseFinalArray++ = *a++;
						}
						char *strx = convert(i,16);
                                                while(*strx!='\0'){
                                                  *traverseFinalArray++ = *strx++;
                                          }
                                                break;				
				case 'p' : l = va_arg(arg,long);
					   	char *b = "0x";
						while(*b!='\0'){
                                                  *traverseFinalArray++ = *b++;
						}
                                        	char *strp = convert(l,16);
                                                while(*strp!='\0'){
                                                  *traverseFinalArray++ = *strp++;
                                          }
                                                break;
			}
		}
		traverse++;
	}
	*traverseFinalArray='\0'; 
	va_end(arg);
	char* string=finalArray;
	while( *string != 0 )
	{
		if(*string=='\n'){
			line++;
			if(line==24){
				scrolldown();
				line=23;
			}
			video = (volatile char*) 0xB8000+(line*160);
			count=0;
			string++;
		} else if (*string=='\r'){
			video = video - 2*count;
			count=0;
			string++;
		}			
		else{
			*video++ = *string++;
			*video++ = colour;
			count++;
		}

		if(count==80){
			line++;
			if(line==24){
				scrolldown();
				line=23;
				video = (volatile char*) 0xB8000+(line*160);
			}
			count=0;
		}
	}
}

void clearLine(char* linePtr){

	for(int i=0;i<80;i++){
		*linePtr++= ' ';
		*linePtr++= colour;	
	}	

}

void scrolldown(){
	int charNum=0;
	char* prevLine;
	for(int lineNum=0;lineNum<=22;lineNum++){
		prevLine = (char*) 0XB8000 + (lineNum*160);
		clearLine((char*) 0XB8000 + (lineNum*160));
		char* nextLine = (char*) 0XB8000 + ((lineNum+1)*160);
		while(charNum!=160){
			*prevLine++=*nextLine++;
			charNum++;
		}
		charNum=0;
	}
	
	video = (volatile char*) 0xB8000+(23*160);
	prevLine = (char*) 0XB8000 + 23*160;
	clearLine(prevLine);
}

char *convert(unsigned int num, int base) 
{ 
	static char r[]= "0123456789ABCDEF";
	static char buffer[50]; 
	char *ptr; 
	
	ptr = &buffer[49]; 
	*ptr = '\0'; 
	
	do 
	{ 
		*--ptr = r[num%base]; 
		num /= base; 
	}while(num != 0); 
	
	return(ptr); 
}


