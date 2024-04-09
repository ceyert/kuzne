#include "stdio.h"
#include "stdlib.h"
#include "string.h"



void demoFunction() {
   
}



int main(int argc, char** argv)
{
    printf("demoFunction addr: %p \n", (void*)&demoFunction);
    printf("main addr: %p \n", (void*)&main);

    {
        int value = 48;
        printf("stack value address: %p \n", &value);
    }

    {
        void* ptr = malloc(512);
        printf("Heap address: %p \n", ptr);
        free(ptr);
    }

    void* sp;
    void* bp;

    __asm__ (
        "mov %%esp, %0\n" // Move current stack pointer to sp
        "mov %%ebp, %1\n" // Move current base pointer to bp
        : "=r"(sp), "=r"(bp) // Output operands
        : // No input operands
        : // No clobbered registers
    );

    // Print SP and BP values
    printf("Current SP: %p\n", sp);
    printf("Current BP: %p\n", bp);


    /*
    Page Fault!
    {
        char* ptr = (char*)0x5fefa8;
        *ptr = 5;
        printf("Value: %i \n", *ptr);
        printf("Value address: %p \n", ptr);
    }
    */

    return 0;
}