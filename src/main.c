#include "structs.h"

/* THE FIRST PART:
	https://www.conradk.com/elf-from-scratch/
	https://linuxvox.com/blog/reading-the-contents-of-an-elf-section-programmatically/
	1. Read header

*/

int read_header(char *name){
	int fp = open(name, O_RDONLY);
	

	return 0;
}


int main(int argc, char **argv){
	read_header(argv[2]);
	return 0;
}
