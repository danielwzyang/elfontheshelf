# Dev Log:

This document must be updated daily every time you finish a work session.

## Daniel Yang

### 2026-05-08: Start of Project
I worked on setting up the devlog files and editing the README.
I also set up the working dirs and continued to look for references.

### 2026-05-10: Learning Some Assembly
I worked on learning a bit of NASM and figuring out how to use syscalls as that's the primary thing our payload will be doing.
I also added my source to the references and made basic hello world and read file programs with some notes.

### 2026-05-11: .text extraction + exec
I figured out how to use nasm and objcopy to extract the bytes that will be injected into the ELF.
I also made a makefile target to automate it.
I also got a basic execve to work to run ls.
My next step is to figure out how to use a socket.

### 2026-05-12: figured out argv + TCP reverse shell
At first I was trying to figure out how to use a socket before realizing that I hadn't worked out argv yet.
For exec.asm previously, I passed a null argv.
I've edited exec.asm to run ls with the -lah flag as a proof of concept that I can run an executable with any arguments.
I was also able to get a reverse shell working by opening a socket and connecting to my digital ocean droplet.

### 2026-05-13: bind shell
Using a similar concept to the reverse shell, I was able to get a bind shell working.
I also made some syntax tweaks to the other payloads.
My next step is to figure out how to download a payload from somewhere using HTTP and run that payload.

### 2026-05-14: download payload
I started to figure out how to use HTTP requests to try to download a binary from a web server that I'd host on the digital ocean droplet.
I also added some syscalls like fchmod that I need to use to get the whole thing working.

### 2026-05-15: use TCP instead of HTTP for the payload
I got a simple bash script payload to work and now thereotically any executable can be ran.
The program also quits if the execve fails.

