[BITS 32]

global _start
extern c_start
extern kuzne_syscall_exit

section .asm

_start:
    call c_start
    call kuzne_syscall_exit     ; isr80h_command9_exit terminate current process & run next task
    ret