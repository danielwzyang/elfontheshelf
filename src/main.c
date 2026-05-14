#include "structs.h"

/* THE FIRST PART:
        https://www.conradk.com/elf-from-scratch/
        https://linuxvox.com/blog/reading-the-contents-of-an-elf-section-programmatically/
        1. Read header

*/

// Coloured Error Printer

int verbrose = 1;

unsigned char placeholder_payload[] = {0x90, 0x90, 0x90, 0x90,
                                       0x90, 0x90, 0x90, 0x90};
uint32_t placeholder_payload_size = sizeof(placeholder_payload);

void nerror(char *error, char *desc, char fatal) {
  fprintf(stderr, "\e[1;91m%s:\e[0m\n", error);
  fprintf(stderr, "\e[0;91m\t|%s\e[0m\n", desc);
  if (fatal)
    exit(1);
}

int read_elf(char *name) {
  /* READING ELF DATA:
          file pointer		fp 	(int)
          memory stats		inf	(struct stat)
          memmory map			mm	(const char *)
          |-> mmap exports file to string, with stats in inf
  */
  int fp = open(name, O_RDONLY);
  struct stat inf;
  if (fp < 0)
    nerror("Open elf", "Failed to open elf file", 1);
  if (fstat(fp, &inf))
    nerror("ELF STAT", "Unable to retreive stats of ELF file", 1);
  const char *mm;
  mm = mmap(NULL, inf.st_size, PROT_READ, MAP_PRIVATE, fp, 0);
  if (verbrose)
    perror("ELF file opened successfully \n");
  // END READ

  // printing all mm bytes

  close(fp);
  munmap((void *)mm, inf.st_size);

  return 0;
}

void write_injection(char *target_path, struct Elf64_Header header,
                     struct InjectionMetadata meta) {
  int fd = open(target_path, O_RDWR);
  if (fd < 0)
    nerror("Write injection", "Failed to open target binary for writing", 1);

  if (lseek(fd, meta.target_padding_offset, SEEK_SET) < 0)
    nerror("Write injection", "Failed to seek to padding offset", 1);

  unsigned char *payload = placeholder_payload;
  uint32_t payload_len =
      meta.payload_size > 0 ? meta.payload_size : placeholder_payload_size;

  if (write(fd, payload, payload_len) != (ssize_t)payload_len)
    nerror("Write injection", "Failed to write payload", 1);

  if (verbrose)
    printf("[+] Wrote %u bytes of payload at file offset 0x%lx\n", payload_len,
           meta.target_padding_offset);

  // ok i need to go back to original herei think
}

int main(int argc, char **argv) {
  printf("ELF Injector Initiated, Target binary: \"%s\"\n", argv[1]);
  read_elf(argv[1]);
  return 0;
}
