#ifndef STRUCT_H
#define STRUCT_H
	#include <stdio.h>
	#include <stdlib.h>
	#include <sys/stat.h>
	#include <sys/mman.h>
	#include <sys/types.h>
	#include <string.h>
	#include <elf.h>
	#include <unistd.h>
	#include <fcntl.h>

struct Elf64_Header {
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint64_t e_entry;
  uint64_t e_phoff;
  uint64_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
};

struct InjectionMetadata {
  uint64_t target_padding_offset;
  uint64_t target_vaddr;
  uint64_t original_entry;
  uint32_t payload_size;
  int text_segment_index;
};

void write_injection(struct Elf64_Header header, struct InjectionMetadata);
int read_header(char *name);

#endif
