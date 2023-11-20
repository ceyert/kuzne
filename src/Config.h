#ifndef CONFIG_H
#define CONFIG_H

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10


#define TOTAL_INTERRUPTS 512


//---------------------------------Block Heap-----------------------------------------

/*
Heap Table Base Address (0x00007E00)    .......
                                        .  1  .  Entry 1 (8 bits)
                                        .......
                                        .  1  .  Entry 2 (8 bits)
                                        .......


Heap Base Address (0x01000000)          ........
                                        . 4096 . Block 1 base address
                                        ........
                                        . 4096 . Block 2 base address
                                        ........
                                        . 4096 . Block 3 base address
                                        ........
*/

//  480.5 KiB avaiable (https://wiki.osdev.org/Memory_Map_(x86))
#define HEAP_TABLE_BASE_ADDRESS 0x00007E00

// Free to use RAM (https://wiki.osdev.org/Memory_Map_(x86))
#define HEAP_BASE_ADDRESS 0x01000000

// 1024 bytes * 1024 = 1048576 (1MB), => 1048576 * 100 = 104857600 (100 MB)
// 100MB available!
#define TOTAL_HEAP_SIZE 104857600

// Each heap block is 4096 bytes. For example: malloc(5000), => 8192 bytes allocated (2 blocks)
#define HEAP_BLOCK_SIZE 4096

// Total 25600 blocks (104857600/4096)
#define TOTAL_BLOCKS TOTAL_HEAP_SIZE / HEAP_BLOCK_SIZE;

//---------------------------------Block Heap-----------------------------------------



#define SECTOR_SIZE 512

#define MAX_FILESYSTEMS 12
#define MAX_FILE_DESCRIPTORS 512

#define MAX_PATH_SIZE 108


#define TOTAL_GDT_SEGMENTS 6


#define PROGRAM_VIRTUAL_ADDRESS 0x400000
#define USER_PROGRAM_STACK_SIZE 1024 * 16
#define PROGRAM_STACK_VIRTUAL_ADDRESS_START 0x3FF000
#define PROGRAM_STACK_VIRTUAL_ADDRESS_END PROGRAM_STACK_VIRTUAL_ADDRESS_START - USER_PROGRAM_STACK_SIZE


#define MAX_PROGRAM_ALLOCATIONS 1024
#define MAX_PROCESSES 12


#define USER_DATA_SEGMENT 0x23
#define USER_CODE_SEGMENT 0x1b

#define MAX_ISR80H_COMMANDS 1024

#define KEYBOARD_BUFFER_SIZE 1024

#endif