#include "structs.h"

/* THE FIRST PART:
	https://www.conradk.com/elf-from-scratch/
	https://linuxvox.com/blog/reading-the-contents-of-an-elf-section-programmatically/
	1. Read header

*/

// Coloured Error Printer

int verbrose = 1; 

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
		|-> mmap exports file to string, with stats in inf
	*/
	int fp = open(name, O_RDONLY); 
	struct stat inf;
	if (fp < 0) 
		nerror("Open elf", "Failed to open elf file", 1);
	if (fstat(fp, &inf)) 
		nerror("ELF STAT", "Unable to retreive stats of ELF file", 1);
	const char *mm;
	mm = mmap(NULL, inf.st_size, PROT_READ, MAP_PRIVATE, fp, 0);
	if(verbrose) perror("ELF file opened successfully \n");
	// END READ 

	// printing all mm bytes
	
	
	close(fp);
	munmap((void *) mm, inf.st_size);
	

	return 0;
}


int main(int argc, char **argv){
	printf("ELF Injector Initiated, Target binary: \"%s\"\n", argv[1]);
	read_elf(argv[1]);
	return 0;
}








