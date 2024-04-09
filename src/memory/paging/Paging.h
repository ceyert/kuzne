/**
 * @file Paging.h
 * @brief Interface and structures for managing paging in a system.
 *
 * Defines constants, structures, and functions for setting up, managing, and utilizing
 * a paging system for memory management. This includes creating page directories,
 * enabling paging, and mapping virtual addresses to physical addresses.
 */

#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PAGING_CACHE_DISABLED  0b00010000 ///< Disable caching for this page.
#define PAGING_WRITE_THROUGH   0b00001000 ///< Enable write-through caching.
#define PAGING_ACCESS_FROM_ALL 0b00000100 ///< Allow access from all privilege levels.
#define PAGING_IS_WRITEABLE    0b00000010 ///< Page is writable.
#define PAGING_IS_PRESENT      0b00000001 ///< Page is present in memory.


/*
 Page Directory
+---------------+       +--------------+
| PD Entry 0    | ----> | Page Table 0 | -----> +----------+      +-------------+
+---------------+       +--------------+        | Page 0   | ---> | Physical    |
| PD Entry 1    | ----> | Page Table 1 |        +----------+      | Memory Page |
+---------------+       +--------------+        | Page 1   |      | (4KB)       |
| PD Entry 2    |       |   ......     |        +----------+      +-------------+
+---------------+       +--------------+        |  ......  |
| ......        |       |Page Table 1023|        +----------+
+---------------+       +--------------+        | Page 1023 |
| PD Entry 1023 |                                +----------+
+---------------+

Each Page Directory Entry points to a Page Table.
Each Page Table contains 1024 Page Entries (for 4KB pages, this maps 4MB of memory).
Each Page Entry maps to a 4KB page in physical memory.

4096 * 1024 = 4194304 (4GB) bytes each page directory entry
*/

#define PAGE_SIZE 4096 ///< Size of each page (4KB).
#define TOTAL_PAGES_PER_TABLE 1024 ///< Number of pages per table.

/**
 * @struct Paging4GbChunk
 * @brief Structure representing a 4GB chunk of memory managed by paging.
 *
 * This structure holds a pointer to the page directory entries used to manage a 4GB chunk of memory.
 */
struct Paging4GbChunk {
    uint32_t *directory_entry; ///< Pointer to the directory entries.
};

/**
 * @brief Creates a new 4GB chunk of memory with paging.
 *
 * @param flags Flags to apply to each page in this chunk.
 * @return struct Paging4GbChunk* Pointer to the new 4GB chunk.
 */
extern struct Paging4GbChunk *paging_new_4gb(uint8_t flags);

/**
 * @brief Switches the current page directory to the one specified by the 4GB chunk.
 *
 * @param directory Pointer to the paging chunk whose directory to switch to.
 */
extern void paging_switch(struct Paging4GbChunk *directory);

/**
 * @brief Enables paging on the system.
 */
extern void enable_paging();

/**
 * @brief Sets a page in the directory.
 *
 * @param directory Pointer to the page directory.
 * @param virt Virtual address to map.
 * @param val Value to set in the page directory entry.
 * @return int Success or failure of the operation.
 */
extern int paging_set(uint32_t *directory, void *virt, uint32_t val);

/**
 * @brief Checks if an address is page aligned.
 *
 * @param addr Address to check.
 * @return bool True if the address is aligned; false otherwise.
 */
extern bool paging_is_aligned(void *addr);

/**
 * @brief Gets the page directory of a 4GB chunk.
 *
 * @param chunk Pointer to the 4GB chunk.
 * @return uint32_t* Pointer to the page directory.
 */
extern uint32_t *paging_4gb_chunk_get_directory(struct Paging4GbChunk *chunk);

/**
 * @brief Frees a 4GB chunk of memory.
 *
 * @param chunk Pointer to the 4GB chunk to free.
 */
extern void paging_free_4gb(struct Paging4GbChunk *chunk);

/**
 * @brief Maps a physical address to a virtual address within a specified directory.
 *
 * @param directory Pointer to the 4GB chunk directory.
 * @param virt Virtual address start.
 * @param phys Physical address start.
 * @param phys_end Physical address end.
 * @param flags Flags for the mapping.
 * @return int Success or failure of the operation.
 */
extern int paging_map_to(struct Paging4GbChunk *directory, void *virt, void *phys, void *phys_end, int flags);

/**
 * @brief Maps a range of physical addresses to virtual addresses.
 *
 * @param directory Pointer to the 4GB chunk directory.
 * @param virt Virtual address start.
 * @param phys Physical address start.
 * @param count Number of pages to map.
 * @param flags Flags for the mapping.
 * @return int Success or failure of the operation.
 */
extern int paging_map_range(struct Paging4GbChunk *directory, void *virt, void *phys, int count, int flags);

/**
 * @brief Maps a single physical address to a virtual address.
 *
 * @param directory Pointer to the 4GB chunk directory.
 * @param virt Virtual address.
 * @param phys Physical address.
 * @param flags Flags for the mapping.
 * @return int Success or failure of the operation.
 */
extern int paging_map(struct Paging4GbChunk *directory, void *virt, void *phys, int flags);

/**
 * @brief Aligns an address to the nearest lower page boundary.
 *
 * @param ptr Address to align.
 * @return void* Aligned address.
 */
extern void *paging_align_address(void *ptr);

/**
 * @brief Retrieves the value of a page directory entry for a virtual address.
 *
 * @param directory Pointer to the page directory.
 * @param virt Virtual address.
 * @return uint32_t Value of the page directory entry.
 */
extern uint32_t paging_get(uint32_t *directory, void *virt);

/**
 * @brief Aligns an address to the lower page boundary.
 *
 * @param addr Address to align.
 * @return void* Aligned address.
 */
extern void *paging_align_to_lower_page(void *addr);

/**
 * @brief Gets the physical address mapped to a given virtual address.
 *
 * @param directory Pointer to the page directory.
 * @param virt Virtual address to query.
 * @return void* Physical address mapped to the virtual address.
 */
extern void *paging_get_physical_address(uint32_t *directory, void *virt);

#endif // PAGING_H