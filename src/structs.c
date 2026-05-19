#include "structs.h"


void f_error(char *error, char *desc) {
  fprintf(stderr, "\e[1;91m%s:\e[0m\n", error);
  fprintf(stderr, "\e[0;91m\t|%s\e[0m\n", desc);
  exit(1);
}

/* GENERATING MEMORY MAP:
		mm		(const char *)	: memory map with mmap
			-> mmap exports file to string
		fname (char *)				: path name
		size	(struct stat)		: size of memory
*/
int file_memmap(const char **mm, char *fname, size_t *size, int perm){
	int fp = open(fname, O_RDONLY);
	struct stat inf;
	if (fp < 0) return -1;
	if (fstat(fp, &inf)) return -2;
	*mm = mmap(NULL, inf.st_size, perm, MAP_PRIVATE, fp, 0);

	//if(verbose) fprintf(stderr, "ELF file opened successfully \n");

	// close file
	close(fp);
	*size = inf.st_size;
	return 0;
}

void mmclean(const char *mm, size_t size){
	// cleaning up mapped memmory
  munmap((void *)mm, size);
}

