#include<sys/elfParser.h>
void parseElf(uint64_t startAddress){
	Elf64_Ehdr* e = (Elf64_Ehdr*)startAddress;
	uint64_t* p= (uint64_t*)e;
	Elf64_Phdr* pHdr,*program;

	if(!elf_check_file((Elf64_Ehdr*)startAddress)){
		kprintf("The file is not an ELF file\n");
		return;
	}	
	kprintf("ELF file found\n");
//	kprintf("Program header offset: %d\n",e->e_phoff);

	pHdr = (Elf64_Phdr*) (p + (e->e_phoff/8));
	kprintf("No of pg hdr %d\n",e->e_phnum);
	
	processPreloading(e->e_entry);
	for(int i =0;i< e->e_phnum;i++){
		program = &pHdr[i];
//		kprintf("Program header type %x\n",program->p_type);
		kprintf("Program header vaddr %x\n",program->p_vaddr);
//		kprintf("Program header paddr %x\n",program->p_paddr);
//		kprintf("Program header filesz %d\n",program->p_filesz);
//		kprintf("Program header memsz  %d\n",program->p_memsz);
		kprintf("Program header offset  %d\n",program->p_offset);
//		kprintf("Program entry point %x\n",e->e_entry);
//		if(program->p_type==1){
		copyToPrcMem((uint64_t)p+program->p_offset,program->p_vaddr,program->p_memsz,e->e_entry);
//		break;
//		}
	}
}

int elf_check_file(Elf64_Ehdr *hdr) {
	if(!hdr) return 0;
	if(hdr->e_ident[EI_MAG0] != ELFMAG0) {
		kprintf("ELF Header EI_MAG0 incorrect.\n");
		return 0;
	}
	if(hdr->e_ident[EI_MAG1] != ELFMAG1) {
		kprintf("ELF Header EI_MAG1 incorrect.\n");
		return 0;
	}
	if(hdr->e_ident[EI_MAG2] != ELFMAG2) {
		kprintf("ELF Header EI_MAG2 incorrect.\n");
		return 0;
	}
	if(hdr->e_ident[EI_MAG3] != ELFMAG3) {
		kprintf("ELF Header EI_MAG3 incorrect.\n");
		return 0;
	}
	return 1;
}

