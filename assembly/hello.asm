section .data
    hello db "Hello, world!", 0xA     ; string with newline
    hello_len equ $ - hello           ; length of the string

section .text
    global _start

_start:
    ; write(1, hello, hello_len)
    mov rax, 1          ; syscall: write
    mov rdi, 1          ; file descriptor: stdout
    mov rsi, hello      ; pointer to message
    mov rdx, hello_len  ; message length
    syscall

    ; exit(0)
    mov rax, 60         ; syscall: exit
    xor rdi, rdi        ; status = 0
    syscall
