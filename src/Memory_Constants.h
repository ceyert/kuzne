#ifndef MEMORY_CONSTANTS_H
#define MEMORY_CONSTANTS_H

//---------------------------------Block Heap-----------------------------------------

/*
Heap Table Base Address (0x00007E00) :  .......
                                        .  1  .  Entry 1 (8 bits)
                                        .......
                                        .  1  .  Entry 2 (8 bits)
                                        .......


Heap Base Address (0x01000000)  :       ........
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

// Each heap block is 4KB (4096 bytes). For example: malloc(5000), => 8192 bytes allocated (2 blocks, 8KB)
#define HEAP_BLOCK_SIZE 4096

// Total 25600 blocks (104857600/4096)
#define TOTAL_BLOCKS TOTAL_HEAP_SIZE / HEAP_BLOCK_SIZE;

//---------------------------------Block Heap-----------------------------------------


//---------------------------------Paging-----------------------------------------

/*
 Page Directory
+---------------+       +--------------+                       Base Addr : 0x01000000
| PD Entry 0    | ----> | Page Table 0 | -----> +----------+      +-------------+
+---------------+       +--------------+        | Page 0   | ---> |             |
| PD Entry 1    | ----> | Page Table 1 |        +----------+      | Page Offset |
+---------------+       +--------------+        | Page 1   |      | (4KB)       |
| PD Entry 2    |       |   ......     |        +----------+      +-------------+
+---------------+       +--------------+        |  ......  |
| ......        |       |Page Table 1023|        +----------+
+---------------+       +--------------+        | Page 1023 |
| PD Entry 1023 |                                +----------+
+---------------+


64 bits virtual address : 
0000 1111 1111 1111 | 1111 1111 1111 1111 | 1111 1111 1111 1111 | 1111 1111 1111 1111

PML4 Index: Bits 47-39
PDPT Index: Bits 38-30
PD Index: Bits 29-21
PT Index: Bits 20-12
Page Offset: Bits 11-0


Each page 4096 bytes (4KB)
Each page table contains 1024 pages 
Each page table can map 1024 * (4096 bytes) = 4MB
*/

#define PAGE_SIZE 4096 ///< Size of page (4KB).
#define TOTAL_PAGES_PER_TABLE 1024 ///< Number of pages per page table.

//---------------------------------Paging-----------------------------------------


//--------------------------------- Pre-Defined User Process Virtual Addresses -----------------------------------------

/*
On protected mode, CPU uses virtual memory by hardware default via MMU.

When user process load/store memory address at 0x400000, CPU translates this memory address to physical memory address.

Process1 load/store : 0x400000 -> 0x120000

Process2 load/store : 0x400000 -> 0x360000

After ELF allocated into memory, kernel must map virtual regions to physical regions. 
*/

// 0x400000 is base virtual address each process(linker.ld) 
#define USER_PROCESS_VIRTUAL_BASE_ADDRESS_NON_ELF 0x400000

#define USER_PROCESS_STACK_SIZE 1024 * 16
#define USER_PROCESS_STACK_VIRTUAL_ADDRESS_BASE 0x3FF000
#define USER_PROCESS_STACK_VIRTUAL_ADDRESS_END USER_PROCESS_STACK_VIRTUAL_ADDRESS_BASE - USER_PROCESS_STACK_SIZE

//--------------------------------- Pre-Defined User Process Virtual Addresses -----------------------------------------


#endif