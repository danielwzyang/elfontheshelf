#ifndef STRUCT_H
#define STRUCT_H
	#include <stdio.h>
	#include <stdlib.h>
	#include <sys/stat.h>
	#include <elf.h>
	#include <sys/mman.h>
	#include <sys/types.h>
	#include <unistd.h>
	#include <fcntl.h>

	struct elf_info{
		void *start_elf;
		void *info_bit_1;
		void *this_is_not_final_owen_add_whatever_you_need;
	};


#endif
