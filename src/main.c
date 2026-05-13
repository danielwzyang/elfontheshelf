#include "structs.h"

/* THE FIRST PART:
	https://www.conradk.com/elf-from-scratch/
	https://linuxvox.com/blog/reading-the-contents-of-an-elf-section-programmatically/
	1. Read header

*/

// Coloured Error Printer
void nerror(char *error, char *desc, char fatal){
	printf(" \e[1;91m%s:\e[0m\n", error);
	printf("\e[0;91m\t|%s\e[0m\n", desc);
	if(fatal) exit(1);
}

int read_header(char *name){
	int fp = open(name, O_RDONLY);
	struct stat inf;
	if (fp < 0) 
		nerror("Open elf", "Failed to open elf file", 1);
	if (fstat(fp, &inf)) 
		nerror("ELF STAT", "Unable to retreive stats of ELF file", 1);

	const char *mm;
	mm = mmap(NULL, inf.st_size, PROT_READ, MAP_PRIVATE, fp, 0);

	return 0;
}


int main(int argc, char **argv){
	printf("ELF Injector Initiated, Target binary: \"%s\"\n", argv[1]);
	read_header(argv[1]);
	return 0;
}








