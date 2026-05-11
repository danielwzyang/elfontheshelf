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

