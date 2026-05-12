#include "structs.h"

/* THE FIRST PART:
	https://www.conradk.com/elf-from-scratch/
	https://linuxvox.com/blog/reading-the-contents-of-an-elf-section-programmatically/
	1. Read header

*/

void nerror(char *s, char *desc){
	printf(" \e[1;31m%s\e[0m\n", s);
	printf("\e[1;31m\t|%s\e[0m\n", desc);
}

int read_header(char *name){
	int fp = open(name, O_RDONLY);
	if (fp < 0){
		nerror("Open elf:", "Failed to open elf file");
	}

	return 0;
}


int main(int argc, char **argv){
	read_header(argv[2]);
	return 0;
}







