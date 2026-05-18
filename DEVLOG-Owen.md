# Dev Log:

This document must be updated daily every time you finish a work session.

## Owen Zeng

### 2026-05-08

Worked On proposal.md for a class period.

### 2026-05-11

Created a structs.h file so that me and Alex can work simultaneously on the ELF parsing and injection for about an hour.

### 2026-05-13

Added writing to elf file based on struct info and payload as well as placeholder values for about 2 hours. Not tested yet.

### 2026-05-14

Trying to set entrypoint back to original and fixing merges for a class period.

### 2026-05-15 

Made write_injection go back to the original and made it seem like the original elf. This is quite untested; about 4 hours. 

### 2026-05-17

Added some stuff in structs.h and makefile so that I can compile since I'm on macOS and don't have the elf.h file natively.

### 2026-05-17

Fixed injection where the paylaod didn't have the correct address, and also the header wasnt updated to have the correct filesize and mem size, so the process wouldn't extend to the payload. 