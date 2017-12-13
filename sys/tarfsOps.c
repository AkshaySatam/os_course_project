#include <sys/tarfsOps.h>
#include <sys/tarfs.h>
#include <sys/kprintf.h>


uint64_t searchTarfs(const char* fName){
	uint64_t* p = (uint64_t*) &_binary_tarfs_start;
	struct posix_header_ustar* t = (struct posix_header_ustar *) p;
	uint64_t size = -1;
	uint64_t n=0;

	while((uint64_t)t < (uint64_t) &_binary_tarfs_end){
		if(compareStrings(fName,t->name)){
//			kprintf("File name: %s \n",t->name);	
			size = decodeTarOctal((unsigned char*)(t->size),sizeof(t->size)/sizeof(char));
//			kprintf("File size: %d\n",size);
			//		kprintf("File uid: %s\n",t->uid);	
			//		kprintf("File uname: %s\n",t->uname);	
			//		kprintf("File version: %s\n",t->version);	
			//		kprintf("File gname: %s\n",t->gname);	
			//		kprintf("File mode: %s\n",t->mode);	
			//		kprintf("File startAddress: %x\n",t);
			return (uint64_t)t;
		}

		if(size%512!=0){
			n = (uint64_t)(size / 512);
			size = (n*512)+512; 	
		}
		t = (struct posix_header_ustar* )((uint64_t) t + size + 512);
	}
//	kprintf("File not found\n");
	return -1;
}

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



void parseTarfs(){
	uint64_t* p = (uint64_t*) &_binary_tarfs_start;
	struct posix_header_ustar* t = (struct posix_header_ustar *) p;
	uint64_t size = -1;
	uint64_t n=0;
//	uint64_t totalPayload=0;
	while((uint64_t)t < (uint64_t) &_binary_tarfs_end){
//		kprintf("Traversing tarfs %x \n",t);
//		kprintf("File name: %s \n",t->name);	
		size = decodeTarOctal((unsigned char*)(t->size),sizeof(t->size)/sizeof(char));
//		kprintf("File size: %d\n",size);
//		kprintf("File uid: %s\n",t->uid);	
//		kprintf("File uname: %s\n",t->uname);	
//		kprintf("File version: %s\n",t->version);	
//		kprintf("File gname: %s\n",t->gname);	
//		kprintf("File mode: %s\n",t->mode);	
//		kprintf("File startAddress: %x\n",t);
//		totalPayload = sizeof(struct posix_header_ustar) + size;

		if(size%512!=0){
			n = (uint64_t)(size / 512);
			size = (n*512)+512; 	
		}
//		kprintf("New size %d \n",size);
		t = (struct posix_header_ustar* )((uint64_t) t + size + 512);
//		kprintf("t: %x \n",t);
	}
}

uint64_t decodeTarOctal(unsigned char* data, int size) {
    unsigned char* currentPtr = data + size;
    uint64_t sum = 0;
    uint64_t currentMultiplier = 1;
    //Skip everything after the last NUL/space character
    //In some TAR archives the size field has non-trailing NULs/spaces, so this is neccessary
    unsigned char* checkPtr = currentPtr; //This is used to check where the last NUL/space char is
    for (; checkPtr >=  data; checkPtr--) {
        if ((*checkPtr) == 0 || (*checkPtr) == ' ') {
            currentPtr = checkPtr - 1;
        }
    }
    for (; currentPtr >=  data; currentPtr--) {
        sum += (*currentPtr-'0') * currentMultiplier;
        currentMultiplier *= 8;
    }
    return sum;
}

