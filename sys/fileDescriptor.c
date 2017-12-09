#include<sys/fileDescriptor.h>
#include<sys/kprintf.h>
#include<sys/tarfsOps.h>

int fd_num = 3;

struct file_descriptor
{
	uint64_t startVAddr;
	uint64_t offset;
	short fd;
	short type;
	short free;	
};

struct file_descriptor* getFreeFd(){
	struct file_descriptor* i;
	i = currentTask->fdArr;

	while(i->free != 1){
		i++;
	}

	i->fd = getFdNum();
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
		initializeFd(fd,file_type,filePtr);
		return fd->fd;

	}
	return -1;
}

int close(int fd){
	return 0;
}

ssize_t read(int fd, void *buf, size_t count){
	memCpy(fd->startVAddress+fd->offset,buf,count);
	return 0;
}

ssize_t write(int fd, const void *buf, size_t count){
	return 0;
}

int getFdNum()
{
        for(; fd_num < 128; fd_num++) {
                if(processID[fd_num] == 0) {
                        processID[fd_num] = 1;
                        return fd_num;
                }
        }
        return -1;
}




