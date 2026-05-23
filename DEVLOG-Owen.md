# Dev Log:

This document must be updated daily every time you finish a work session.

## Owen Zeng

### 2026-05-08 Proposal

Worked On proposal.md for a class period.

### 2026-05-11 creating structs file based on elf

Created a structs.h file so that me and Alex can work simultaneously on the ELF parsing and injection for about an hour.

### 2026-05-13  adding writing to elf, and placeholder values

Added writing to elf file based on struct info and payload as well as placeholder values for about 2 hours. Not tested yet.

### 2026-05-14  fix merges, fixing entry point logic

Trying to set entrypoint back to original address and fixing merges for a class period.

### 2026-05-15 fixed writing injection

Made write_injection go back to the original and made it seem like the original elf. This is quite untested; about 4 hours. 

### 2026-05-17 macOS compiling and libraries

Added some stuff in structs.h and makefile so that I can compile since I'm on macOS and don't have the elf.h file natively.

### 2026-05-17 fixing injection and header modification

Fixed injection where the paylaod didn't have the correct address, and also the header wasnt updated to have the correct filesize and mem size, so the process wouldn't extend to the payload.

### 2025-05-18 merge fixes
merged with alex and fixed weird merge errors that affected his read_elf code

### 2026-05-20 script

Finished script

### 2026-05-21 editting
edited script for accuracy

### 2026-05-22 recording
recording my sections