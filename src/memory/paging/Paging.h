#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PAGING_CACHE_DISABLED  0b00010000
#define PAGING_WRITE_THROUGH   0b00001000
#define PAGING_ACCESS_FROM_ALL 0b00000100
#define PAGING_IS_WRITEABLE    0b00000010
#define PAGING_IS_PRESENT      0b00000001


#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096


struct Paging4GbChunk {
    uint32_t *directory_entry;
};

extern struct Paging4GbChunk *paging_new_4gb(uint8_t flags);

extern void paging_switch(struct Paging4GbChunk *directory);

extern void enable_paging();

extern int paging_set(uint32_t *directory, void *virt, uint32_t val);

extern bool paging_is_aligned(void *addr);

extern uint32_t *paging_4gb_chunk_get_directory(struct Paging4GbChunk *chunk);

extern void paging_free_4gb(struct Paging4GbChunk *chunk);

extern int paging_map_to(struct Paging4GbChunk *directory, void *virt, void *phys, void *phys_end, int flags);

extern int paging_map_range(struct Paging4GbChunk *directory, void *virt, void *phys, int count, int flags);

extern int paging_map(struct Paging4GbChunk *directory, void *virt, void *phys, int flags);

extern void *paging_align_address(void *ptr);

extern uint32_t paging_get(uint32_t *directory, void *virt);

extern void *paging_align_to_lower_page(void *addr);

extern void *paging_get_physical_address(uint32_t *directory, void *virt);

#endif