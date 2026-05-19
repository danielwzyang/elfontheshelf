# Dev Log:

This document must be updated daily every time you finish a work session.

## Alex Shao

### 2026-05-08 - README & Group Project Name
- Discussed project dynamics
- Started README

### 2026-05-12 - Starting ELF injector
- Worked on main elf infecter, opening the ELF file
- Created a testing makefile
- created struct.h temporary file 

### 2026-05-13 - Testing binary, reading ELF
- Worked on main elf infecter, setting up test binaries to read
- Improve makefile with more testing functionality
- extract values from the ELF file
- Error detection and printing


### 2026-05-14 - Comments, verification, Elf header extraction
- Better comments for reading ELF
- ELF file authenticity verification
- started with ELF header interaction

### 2026-05-15 - Elf header interactions, program header extraction
- Extracted header section of ELF file
- Extract*(ing) programs from header section
- Retrieving program data

### 2026-05-16 & 17 - Program header fixes, calculations
- Fix program extraction (Use correct offset for struct cast) 
- Program data calculation (checking if program is valid for injection)
- Extracting injection address


### 2026-05-18 - Organisation with file reading
- Separated file reading from read portion to be able to use on reading payload
- Revamped write function to allow for writing
- - Configured entry point and implemented machine jump-back function
- organised comments

