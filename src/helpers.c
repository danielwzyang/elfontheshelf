#include "structs.h"

int verbose = 0;

// Fatal error printer
void f_error(const char *error, const char *desc) {
    fprintf(stderr, "\e[1;91m%s:\e[0m\n", error);
    fprintf(stderr, "\e[0;91m\t|%s\e[0m\n", desc);
    exit(1);
}

void parse_args(int argc, char **argv, char **target, char **payload){
	for(int i = 0; i < argc; ++i){
		if(!strcmp(argv[i], "-p")){ 
			if(i+1 > argc) goto no_pay_error; 
			i ++;
			*payload = argv[i];
		}else if(!strcmp(argv[i], "-t")){
			if(i+1 > argc) goto no_tar_error;
			i ++;
			*target = argv[i];
		}
		if(!strcmp(argv[i], "-v")) verbose ++;
	}
	if(*target == NULL) goto no_tar_error;
	if(*payload == NULL) goto no_pay_error;

	return;
	no_pay_error:
	f_error("PROGRAM INPUT", "No payload inputted.");
	no_tar_error:
	f_error("PROGRAM INPUT", "No target inputted.");

}

/* GENERATING MEMORY MAP:
		mm		(const char *)	: memory map with mmap
			-> mmap exports file to string (virtual memory
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

