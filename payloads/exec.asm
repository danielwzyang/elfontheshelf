[bits 64]

global _start

_start:
    ; first fork
    mov rax, 58 ; sys_fork = 58
    syscall

    test rax, rax ; check if rax is 0
    jnz hello ; if pid isn't 0 then we are the parent so let's print hello

    ; execve(file, argv, null)
    xor rax, rax ; set rax to 0
    push rax ; stack now has NULL
    lea rax, [rel flag]
    push rax ; stack now has flag and NULL
    lea rdi, [rel path] ; set path
    push rdi ; stack has path, flag, and NULL
    mov rsi, rsp
    xor rdx, rdx ; null envp
    mov rax, 59 ; sys_execve = 59
    syscall

; read hello.asm
hello:
    mov rax, 1
    mov rdi, 1
    lea rsi, [rel text]
    mov rdx, 13
    syscall

    mov rax, 0xDEADBEEFDEADBEEF
    jmp rax

text:
    db "Hello world!", 0Ah

path:
    db "/usr/bin/ls", 0

flag:
    db "-lah", 0

