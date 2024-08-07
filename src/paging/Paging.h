/**
 * @file Paging.h
 * @brief Interface and structures for managing paging in a system.
*/




#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "Memory_Constants.h"

#define PAGING_CACHE_DISABLED  0b00010000 ///< Disable caching for this page.
#define PAGING_WRITE_THROUGH   0b00001000 ///< Enable write-through caching.
#define PAGING_ACCESS_FROM_ALL 0b00000100 ///< Allow access from all privilege levels.
#define PAGING_IS_WRITEABLE    0b00000010 ///< Page is writable.
#define PAGING_IS_PRESENT      0b00000001 ///< Page is present in memory.

// Mask the lower 12 bits of a 32-bit address as offset. 
// These lower 12 bits are used for the offset within a page. 
// Using this mask with a bitwise AND operation (&) clears the lower 12 bits of an address.
#define MASK_12_BITS_PAGE 0xFFFFF000    // 11111111111111111111000000000000

/**
 * @struct PageDirectory
 * @brief Structure representing a 4GB chunk of memory managed by paging.
 *
 * This structure holds a pointer to the page directory entries used to manage a 4GB chunk of memory.
 */
 
struct PageDirectory {
    uint32_t *directory_entry_ptr; ///< Pointer to the directory entries.
};

/**
 * @brief Allocates page directory for 4GB address mapping.
 *
 * @param flags Flags to apply to each page in this chunk.
 * @return struct PageDirectory* Pointer to the new 4GB chunk.
 */
extern struct PageDirectory *new_page_directory_allocation(uint8_t flags);

/**
 * @brief Switches the current page directory to the one specified by the 4GB chunk.
 *
 * @param directory Pointer to the paging chunk whose directory to switch to.
 */
extern void set_current_page_directory(struct PageDirectory *directory);

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
extern uint32_t *paging_4gb_chunk_get_directory(struct PageDirectory *chunk);

/**
 * @brief Frees a 4GB chunk of memory.
 *
 * @param chunk Pointer to the 4GB chunk to free.
 */
extern void paging_free_4gb(struct PageDirectory *chunk);

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
extern int paging_map_to(struct PageDirectory *directory, void *virt, void *phys, void *phys_end, int flags);

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
extern int paging_map_range(struct PageDirectory *directory, void *virt, void *phys, int count, int flags);

/**
 * @brief Maps a single physical address to a virtual address.
 *
 * @param directory Pointer to the 4GB chunk directory.
 * @param virt Virtual address.
 * @param phys Physical address.
 * @param flags Flags for the mapping.
 * @return int Success or failure of the operation.
 */
extern int map_virtual_address_to_physical_address(struct PageDirectory *directory, void *virt, void *phys, int flags);

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