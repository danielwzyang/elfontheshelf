# Elf Injection (temporary)

### Advent of Coders

Alex Shao, Daniel Yang, Owen Zeng

### Project Description:

Executable binaries can be stored in a format known as ELF.
You can exploit the way that the binary is organized to inject malicious code into zeroed out bytes that aren't used.

### Instructions:

This is meant to be run on Linux as other operating systems use different executable formats.
Additionally, certain required tools such gcc, ld, and objcopy are built into Linux.
NASM and make are the only separate tools that need to be installed.

#### Payloads

To use the payloads, the makefile has the targets _payload_ and _test_payload_
The payload target will generate a .bin of the .text section of the compiled payload.
This contains the bytes that will be injected into the ELF.
The test_payload target will just run the payload to see what is supposed to happen if it were run properly after injection.

Both of these targets will accept an ASM parameter for the source file.

Some of the source files will also take an IP and PORT parameter to customize where an endpoint would be.

The following are what each payload does and how to use them:

**exec.asm**
This will execute `ls -lah` to print all the files.
It can be compiled using the following command:

```bash
$ make payload ASM=payloads/exec.asm
```

**bind_shell.asm**
This will open a TCP socket at a port specified by the PORT parameter (or 9000 by default).
From an attacker shell, you can connect to the machine's ip with the port and get a bind shell.



### Resources/ References:

http://ouah.org/elf-pv.txt

https://hckng.org/~isra/articles/house-of-pain.html

https://www.man7.org/linux/man-pages/

https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/

https://www.exploit-db.com/shellcodes

[![assignment due date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/B2vtqcJe)
