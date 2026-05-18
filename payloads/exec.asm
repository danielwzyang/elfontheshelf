[bits 64]

global _start

_start:
    ; first fork
    mov rax, 57 ; sys_fork = 57
    syscall

    test rax, rax ; check if rax is 0
    jnz original

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

    ; if execve failed exit
    mov rax, 60
    xor rdi, rdi
    syscall

original:
    mov rax, 0xDEADBEEFDEADBEEF
    jmp rax

path:
    db "/usr/bin/ls", 0

flag:
    db "-lah", 0

