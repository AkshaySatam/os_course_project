#include <sys/kprintf.h>


//int valid[10] ;
char sys_buffer_write[10][128];
//char**  sys_buffer_read;

void initialize(){
	sbuffer=0;
	soffset=0;
	read_ptr=0;

	for(int i = 0; i <10;i++){
		valid[i] =0;
	}

	for(int i = 0;i<10;i++){
		for(int j = 0;j<128;j++){
			sys_buffer_write[i][j] = '\0';
		}
	}
}

void term_write(char c){
//	initialize();
	char sys_c = c;
	sys_buffer_write[soffset][sbuffer++] = sys_c;
	kprintf("%c",sys_c);
	if(sys_c == '\n')
	{
		valid[soffset] = 1;
	 	soffset++;
		soffset = soffset % 10;
		sbuffer = 0;
	}					
}


void term_read(char* buf, int read_index,int length){
//	kprintf("In term read\n");
	int i=0;
	for(i = 0;i < length;i++){
		buf[i] = sys_buffer_write[read_index][i];
	}
	buf[i]='\0';	
			valid[read_ptr]=0;
			read_ptr++;
			read_ptr = read_ptr % 10;
			//TODO Need to accomodate for offset overflow
//			kprintf("%s\n",buf);
}		



