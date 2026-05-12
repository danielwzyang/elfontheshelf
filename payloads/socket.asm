[bits 64]

global _start

_start:
    ; fork()
    mov rax, 58 ; sys_fork = 58
    syscall

    test rax, rax ; check if rax is 0
    jnz hello ; if pid isn't 0 then we are the parent so let's print hello

    ; execve(file, {file, NULL}, NULL)
    lea rdi, [rel path]
    push rdi ; argv now has path
    xor rax, rax ; get NULL
    push rax ; argv now has path and NULL
    mov rdi, rsp ; stack is pointing to argv
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
    db "/usr/bin/zsh", 0
