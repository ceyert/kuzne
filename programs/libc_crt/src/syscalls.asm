[BITS 32]

section .asm

global print:function
global kuzne_getkey:function
global kuzne_malloc:function
global kuzne_free:function
global kuzne_putchar:function
global kuzne_process_load_start:function
global kuzne_process_get_arguments:function 
global kuzne_system:function
global kuzne_exit:function

; void print(const char* filename)
print:
    push ebp
    mov ebp, esp
    push dword[ebp+8]
    mov eax, 1 ; Command print
    int 0x80
    add esp, 4
    pop ebp
    ret

; int kuzne_getkey()
kuzne_getkey:
    push ebp
    mov ebp, esp
    mov eax, 2 ; Command getkey
    int 0x80
    pop ebp
    ret

; void kuzne_putchar(char c)
kuzne_putchar:
    push ebp
    mov ebp, esp
    mov eax, 3 ; Command putchar
    push dword [ebp+8] ; Variable "c"
    int 0x80
    add esp, 4
    pop ebp
    ret

; void* kuzne_malloc(size_t size)
kuzne_malloc:
    push ebp
    mov ebp, esp
    mov eax, 4 ; Command malloc
    push dword[ebp+8] ; Variable "size"
    int 0x80
    add esp, 4
    pop ebp
    ret

; void kuzne_free(void* ptr)
kuzne_free:
    push ebp
    mov ebp, esp
    mov eax, 5 ; Command 5 free
    push dword[ebp+8] ; Variable "ptr"
    int 0x80
    add esp, 4
    pop ebp
    ret

; void kuzne_process_load_start(const char* filename)
kuzne_process_load_start:
    push ebp
    mov ebp, esp
    mov eax, 6 ; Command 6 process load start ( stars a process )
    push dword[ebp+8] ; Variable "filename"
    int 0x80
    add esp, 4
    pop ebp
    ret

; int kuzne_system(struct command_argument* arguments)
kuzne_system:
    push ebp
    mov ebp, esp
    mov eax, 7 ; Command 7 process_system ( runs a system command based on the arguments)
    push dword[ebp+8] ; Variable "arguments"
    int 0x80
    add esp, 4
    pop ebp
    ret


; void kuzne_process_get_arguments(struct process_arguments* arguments)
kuzne_process_get_arguments:
    push ebp
    mov ebp, esp
    mov eax, 8 ; Command 8 Gets the process arguments
    push dword[ebp+8] ; Variable arguments
    int 0x80
    add esp, 4
    pop ebp
    ret

; void kuzne_exit()
kuzne_exit:
    push ebp
    mov ebp, esp
    mov eax, 9 ; Command 9 process exit
    int 0x80
    pop ebp
    ret