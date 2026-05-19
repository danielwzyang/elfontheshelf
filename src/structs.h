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

struct InjectionMetadata {
  uint64_t target_padding_offset;
  uint64_t target_vaddr;
  uint64_t original_entry;
  uint32_t payload_size;
  int text_segment_index;
};

void f_error(char *error, char *desc);
int file_memmap(const char **mm, char *fname, size_t *size, int perm);
void mmclean(const char *mm, size_t size);



#endif
