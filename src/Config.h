#ifndef CONFIG_H
#define CONFIG_H

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10


#define PEACHOS_TOTAL_INTERRUPTS 512


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
// Total 25600 blocks (104857600/4096)
#define HEAP_BLOCK_SIZE 4096

//---------------------------------Block Heap-----------------------------------------



#define PEACHOS_SECTOR_SIZE 512

#define PEACHOS_MAX_FILESYSTEMS 12
#define PEACHOS_MAX_FILE_DESCRIPTORS 512

#define PEACHOS_MAX_PATH 108

#define PEACHOS_TOTAL_GDT_SEGMENTS 6

#define PEACHOS_PROGRAM_VIRTUAL_ADDRESS 0x400000
#define PEACHOS_USER_PROGRAM_STACK_SIZE 1024 * 16
#define PEACHOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START 0x3FF000
#define PEACHOS_PROGRAM_VIRTUAL_STACK_ADDRESS_END PEACHOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START - PEACHOS_USER_PROGRAM_STACK_SIZE

#define PEACHOS_MAX_PROGRAM_ALLOCATIONS 1024
#define PEACHOS_MAX_PROCESSES 12

#define USER_DATA_SEGMENT 0x23
#define USER_CODE_SEGMENT 0x1b

#define PEACHOS_MAX_ISR80H_COMMANDS 1024

#define PEACHOS_KEYBOARD_BUFFER_SIZE 1024

#endif