#ifndef STRUCT_H
#define STRUCT_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>

#ifdef __APPLE__
#include <libelf/gelf.h>
#else
#include <elf.h>
#endif

#define JMP_INSTRUCTION_SIZE 5

extern int verbose; // extern = global basically

void f_error(const char *error, const char *desc);
int file_memmap(char **mm, const char *fname, size_t *size, int perm);
void mmclean(const char *mm, size_t size);

#endif
