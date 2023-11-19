#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char **argv) {

    int value = 48;
    printf("Value: %i \n", value);

    void *ptr = malloc(512);
    printf("Allocated memory: %p \n", ptr);
    printf("Allocated memory: %p \n", &value);
    free(ptr);


    return 0;
}