#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main(int argc, char** argv)
{
    {
        int value = 48;
        printf("stack value address: %p \n", &value);
    }

    {
        void* ptr = malloc(512);
        printf("Heap address: %p \n", ptr);
        free(ptr);
    }

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