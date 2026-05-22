### Presentation format (Taken from Google Docs script structure template)

# How ELF works + layout
- Used by linux
- Divided into sections
- Program header table tells the OS how to load the file into memory
- Entry point field tells the OS where to start executing instructions
 
# Padded Sections + How to Abuse It
- Sections are typically aligned to page boundaries (usually 4096 bytes)
- Causes gaps of zeroed out bytes between sections that aren’t used
- Bytes are loaded into memory but never executed
 
# How the Injector Works
- Parse ELF header to find gaps
- Copy payload bytes into a gap that’s big enough
- Change entry point to the beginning of the payload
- Append jump instruction to go back to original entry point
- Payload runs first and then original binary continues normally
 
# Assembly + Syscalls
- Linux exposes some kernel functions through syscalls
- Assembly allows you to call them without any libraries or dependencies
- In x86_64 assembly you can trigger one using the syscall instruction
- This is how the payloads do everything including opening sockets, running executables, and downloading files
 
# Virtual Memory, ASLR, No-PIE
- The kernel will give every process its own space in virtual memory
- ASLR will randomize where the code is stored in memory each time
- For the kernel ASLR is enabled by default meaning shared libraries will be loaded in random addresses
- This isn’t on for binaries by default, but a binary can be denoted as a PIE (position independent executable) which will enable ASLR for itself
- Our hardcoded jump addresses will only work if the binary loads at a fixed address or if the injected payload is in the same pagesegment as the original entry point
- This is why we compile our target with no pie (but it will also work with pie in certain cases)
 
# Demo (download payload)
- Show the unmodified target binary running normally
- Run the injector on it
- Set up netcat on the attacking machine to serve a bash script
- Run the infected binary
 
# Explain why download marginalizes the assembly barrier
- Writing shellcode is hard because assembly is pretty hard to debug
- It’s also limited by size based on how big the gap is
- This download payload is tiny since it just opens up a socket, reads the bytes, and writes it to a file before executing it
- This gives the attacker basically unlimited complexity
 
# Why this is mostly not a problem in today’s world
- PIE is enabled by default on modern Linux distributions
- ASLR makes hardcoded address unreliable
- Antiviruses and tools can also check if entry points were modified
- This is mostly for educational purposes nowadays
 
# Takeaways
- ELF’s design prioritizes performance over security
- Small design choices can be exploited
- Understanding how binary formats work is really important for understanding low-level security
 
