#include<sys/fileDescriptor.h>
#include<sys/kprintf.h>
#include<sys/tarfsOps.h>

struct file_descriptor* getFreeFd(){
	struct file_descriptor* i;
	i = currentTask->fdArr;
	int count = 0;
	while(i->free != 1){
		i++;
		count++;
	}

	i->fd = count;
	return i;
}

void initializeFd(struct file_descriptor* fd, int file_type, uint64_t filePtr){
	
		fd->free = 1;
		fd->type = file_type;
		fd->offset = 0;
		fd->startVAddr = filePtr+512;
}

int open(const char *pathname, int flags){
	struct file_descriptor* fd;
	uint64_t filePtr = searchTarfs(pathname);
	if(filePtr != -1){
		fd = getFreeFd();
		initializeFd(fd,1,filePtr);
		return fd->fd;
	}
	kprintf("File not found\n");
	return -1;
}

int close(int fd){
	struct file_descriptor * fd1  = currentTask->fdArr;
	struct file_descriptor * fdPtr  =  (struct file_descriptor*)(fd1 + fd);
	fdPtr->free=1;
	return 0;
}

void copyBytes2(uint64_t src, uint64_t dest, uint64_t size){
        char* s = (char*) src;
        char* d = (char*) dest; 
        int count=0;
        while(count<size){
                *d++=*s++;
                count++;
        }
        *d='\0';
}

ssize_t fileRead(int fd, void *buf, size_t count){
	struct file_descriptor * fd1  = currentTask->fdArr;
	struct file_descriptor * fdPtr  =  (struct file_descriptor*)(fd1 + fd);
	copyBytes2(fdPtr->startVAddr+fdPtr->offset,(uint64_t)buf,count);
	fdPtr->offset = fdPtr->offset + count;
	return count;
}

ssize_t write(int fd, const void *buf, size_t count){
	return 0;
}

