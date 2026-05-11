[bits 64]

global _start

_start:
    ; first fork
    mov rax, 58 ; sys_fork = 58
    syscall

    test rax, rax ; check if rax is 0
    jnz hello ; if pid isn't 0 then we are the parent so let's print hello

    ; execve(file, null, null)
    mov rax, 59 ; sys_execve = 59
    lea rdi, [rel path]
    xor rsi, rsi ; null argv
    xor rdx, rdx ; null envp
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
