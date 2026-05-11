[bits 64]

global _start

_start:
    ; first step is to open the file 
    mov rax, 2 ; sys_open = 2
    lea rdi, [rel filename] ; filename
    xor rsi, rsi ; flag is 0 for O_RDONLY - see flags in man 2 open
    syscall ; the file descriptor will be stored in rax

    ; using the file descriptor we can read the file
    mov rdi, rax ; move fd first before setting rax
    xor rax, rax ; sys_read = 0
    lea rsi, [rel buffer] ; set buffer for reading
    mov rdx, 32 ; 32 byte buffer
    syscall ; bytes read will be stored in rax

    ; now we can write out the buffer
    mov rdx, rax ; move bytes read first 
    mov rax, 1 ; sys_write = 1
    mov rdi, 1 ; fd for stdout is 1
    lea rsi, [rel buffer] ; buffer
    syscall ; 

    ; it would be base (wherever ASLR puts the program) + original e_entry = actual address
    ; this dummy address would be replaced by the injector with the right address
    mov rax, 0xDEADBEEFDEADBEEF ; if you run this standalone it will segfault because deadbeef is unfortunately not a valid address
    jmp rax

filename: db "/etc/hostname", 0 ; file we want to read that's null terminated
buffer: times 32 db 0 ; reserve 32 zeroed out bytes for reading
