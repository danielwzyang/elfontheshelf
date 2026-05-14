#include "structs.h"

/* THE FIRST PART:
	https://www.conradk.com/elf-from-scratch/
	https://linuxvox.com/blog/reading-the-contents-of-an-elf-section-programmatically/
	1. Read header

*/

// Coloured Error Printer

int verbose = 1; 

void nerror(char *error, char *desc, char fatal){
	fprintf(stderr, "\e[1;91m%s:\e[0m\n", error);
	fprintf(stderr, "\e[0;91m\t|%s\e[0m\n", desc);
	if(fatal) exit(1);

}

int read_elf(char *name){
	/* READING ELF DATA:
		file pointer		fp 	(int)
		memory stats		inf	(struct stat)
		memmory map			mm	(const char *)
		-> mmap exports file to string, with stats in inf
	*/
	int fp = open(name, O_RDONLY); 
	struct stat inf;
	if (fp < 0) 
		nerror("OPEN ELF", "Failed to open elf file", 1);
	if (fstat(fp, &inf)) 
		nerror("ELF STAT", "Unable to retreive stats of ELF file", 1);
	const char *mm = mmap(NULL, inf.st_size, PROT_READ, MAP_PRIVATE, fp, 0);
	if(verbose) fprintf(stderr, "ELF file opened successfully \n");

	// END READ 


	// Verifying authenticity & type of ELF file
		// verify authenticity
	if( mm[EI_MAG0] != ELFMAG0 ||
			mm[EI_MAG1] != ELFMAG1 ||
			mm[EI_MAG2] != ELFMAG2 ||
			mm[EI_MAG3] != ELFMAG3   )
		nerror("ELF FILE", "Binary is not an ELF file, failed ELF header validation", 1);
		// verify 64 bit
	if( mm[EI_CLASS] != ELFCLASS64 )
		nerror("ELF FILE", "Binary is not 64 bit, failed ELF header validation", 1);
	else if(verbose) fprintf(stderr, "ELF file verified.\n");
	// END VERIFY
		
	// ELF header
	Elf64_Ehdr *header = (Elf64_Ehdr *) mm;

	if( header->e_phoff == 0 )
		nerror("ELF Header", "ELF Program Header struct brokeen?", 1);
	else if( header->e_shoff == 0 )
		nerror("ELF Header", "ELF Section Header struct brokeen?", 1);
	else if(verbose){
		fprintf(stderr, "Elf program header byte pos: %lu\n", header->e_phoff);
		fprintf(stderr, "Elf section header byte pos: %lu\n", header->e_shoff);
	}
	
	unsigned int phsize = header->e_phentsize;
	unsigned int phnum = header->e_phnum;
	if(verbose){
		fprintf(stderr, "Program header entries: %d\n", phnum);
		fprintf(stderr, "Program header entry byte size: %d\n", phsize);

		fprintf(stderr, "Program header entries:");
		for(int i = 0; i < phnum; ++i){
			for(int j = 0; j < 27; ++j)
				printf("%c", mm[header->e_phoff+(phnum*phsize)+j]);
			printf("\n");
		}
	}
	fprintf(stderr, "Elf header table: %x\n", mm[header->e_phoff]);

	// Program Header
	
	
		
	// cleaning up memmory
	close(fp);
	munmap((void *) mm, inf.st_size);
	

	return 0;
}

void write_injection(struct Elf64_Header header, struct InjectionMetadata){
		
}

int main(int argc, char **argv){
	printf("ELF Injector Initiated, Target binary: \"%s\"\n", argv[1]);
	read_elf(argv[1]);
	return 0;
}

