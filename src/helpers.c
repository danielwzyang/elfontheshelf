#include "structs.h"

int verbose = 0;

void f_error(const char *error, const char *desc) {
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

int file_memmap(char **mm, const char *fname, size_t *size, int perm){
    int flags = (perm & PROT_WRITE) ? O_RDWR : O_RDONLY; // we don't need to write into the payload that would just be read only
	int fd = open(fname, flags);
	if (fd < 0) return -1;

	struct stat inf;
	if (fstat(fd, &inf)) {
        close(fd);
        return -2;
    }
	*mm = mmap(NULL, inf.st_size, perm, MAP_SHARED, fd, 0);
    close(fd);

	if(*mm == MAP_FAILED) return -3;

	*size = inf.st_size;
	return 0;
}

void mmclean(const char *mm, size_t size){
    // cleaning up mapped memmory
    munmap((void *)mm, size);
}

