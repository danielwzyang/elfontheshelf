[bits 64]

; basic idea is to use syscalls
; https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/

global _start ; global entry point for ld

_start:
    mov rax, 1 ; code for write
    mov rdi, 1 ; fd = 1 for stdout
    lea rsi, [rel text] ; pass relative address of the string to buffer
    mov rdx, 13 ; count is 13 bytes
    syscall ; will use the registers above to run a certain syscall

    ; look at read_file.asm for an explanation of the below
    mov rax, 0xDEADBEEFDEADBEEF
    jmp rax

text:
    db "Hello world!", 0x0A ; define string and 0x0A is new line
