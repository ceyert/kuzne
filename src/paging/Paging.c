#include "Paging.h"
#include "Status.h"
#include "malloc/Kheap.h"

void paging_load_directory(uint32_t* directory);

static uint32_t* CURRENT_PAGE_DIRECTORY_ = 0;

/*
Enables 4GB virtual memory addressing. (32 bits memory addresses) 
Allocates 4MB for page entries.

This function allocates memory for 4GB page directory that used to address accessibility. 
No frame(4KB) allocation yet. No mapping v_address to p_address yet.
*/

struct PageDirectory* new_page_directory_allocation(uint8_t flags)
{
    // 1024 page tables
    uint32_t* newPageDirectory = kernel_zeroed_alloc(sizeof(uint32_t) * TOTAL_PAGES_PER_TABLE);

    int offset_4_mb = 0;

    // Allocate 1024 page tables
    for (int page_table_idx = 0; page_table_idx < TOTAL_PAGES_PER_TABLE; page_table_idx++)
    {
        // Allocate 1024 pages
        uint32_t* page_table_ptr = kernel_zeroed_alloc(sizeof(uint32_t) * TOTAL_PAGES_PER_TABLE);

        // Maps page table with 4MB(1024 * 4KB) physical address offsets. 
        for (int page_idx = 0; page_idx < TOTAL_PAGES_PER_TABLE; page_idx++)
        {
            // Page1 : 0 to 4 KB offset
            // Page2 : 4 to 8 KB offset
            // Page3 : 8 to 12 KB offset

            // Map page with 4KB offset
            page_table_ptr[page_idx] = (offset_4_mb + (page_idx * PAGE_SIZE)) | flags;
        }

        offset_4_mb += (TOTAL_PAGES_PER_TABLE * PAGE_SIZE); // move page table offset 4MB

        newPageDirectory[page_table_idx] = (uint32_t)page_table_ptr | flags | PAGING_IS_WRITEABLE;
    }

    // 1024 page tables (page directory)
    // Each page table 1024 pages
    // Each page 4KB offset
    // 1024 * 1024 * 4096 = 4294967296 (4GB)

    // 4KB(4096) allocation each malloc
    // 1024 iterations
    // 1024 * 4096 = 4MB malloc memory allocation in total

    struct PageDirectory* chunk_4gb = kernel_zeroed_alloc(sizeof(struct PageDirectory));
    chunk_4gb->directory_entry_ptr = newPageDirectory;
    return chunk_4gb;
}

void set_current_page_directory(struct PageDirectory* directory)
{
    paging_load_directory(directory->directory_entry_ptr);
    CURRENT_PAGE_DIRECTORY_ = directory->directory_entry_ptr;
}

void paging_free_4gb(struct PageDirectory* chunk)
{
    for (int i = 0; i < 1024; i++)
    {
        uint32_t entry = chunk->directory_entry_ptr[i];
        uint32_t* table = (uint32_t*)(entry & 0xfffff000);
        kernel_free_alloc(table);
    }

    kernel_free_alloc(chunk->directory_entry_ptr);
    kernel_free_alloc(chunk);
}

uint32_t* paging_4gb_chunk_get_directory(struct PageDirectory* chunk)
{
    return chunk->directory_entry_ptr;
}

bool paging_is_aligned(void* addr)
{
    return ((uint32_t)addr % PAGE_SIZE) == 0;
}

int paging_get_indexes(void* virtual_address, uint32_t* directory_index_out, uint32_t* table_index_out)
{
    int res = 0;
    if (!paging_is_aligned(virtual_address))
    {
        res = -EINVARG;
        goto out;
    }

    *directory_index_out = ((uint32_t)virtual_address / (TOTAL_PAGES_PER_TABLE * PAGE_SIZE));
    *table_index_out =
        ((uint32_t)virtual_address % (TOTAL_PAGES_PER_TABLE * PAGE_SIZE) / PAGE_SIZE);
out:
    return res;
}

void* paging_align_address(void* ptr)
{
    if ((uint32_t)ptr % PAGE_SIZE)
    {
        return (void*)((uint32_t)ptr + PAGE_SIZE - ((uint32_t)ptr % PAGE_SIZE));
    }

    return ptr;
}

void* paging_align_to_lower_page(void* addr)
{
    uint32_t _addr = (uint32_t)addr;
    _addr -= (_addr % PAGE_SIZE);
    return (void*)_addr;
}

int map_virtual_address_to_physical_address(struct PageDirectory* directory, void* virt, void* phys, int flags)
{
    if (((unsigned int)virt % PAGE_SIZE) || ((unsigned int)phys % PAGE_SIZE))
    {
        return -EINVARG;
    }

    return paging_set(directory->directory_entry_ptr, virt, (uint32_t)phys | flags);
}

int paging_map_range(struct PageDirectory* directory, void* virt, void* phys, int count, int flags)
{
    int res = 0;
    for (int i = 0; i < count; i++)
    {
        res = map_virtual_address_to_physical_address(directory, virt, phys, flags);
        if (res < 0) break;
        virt += PAGE_SIZE;
        phys += PAGE_SIZE;
    }

    return res;
}

int paging_map_to(struct PageDirectory* directory, void* virt, void* phys, void* phys_end, int flags)
{
    int res = 0;
    if ((uint32_t)virt % PAGE_SIZE)
    {
        res = -EINVARG;
        goto out;
    }
    if ((uint32_t)phys % PAGE_SIZE)
    {
        res = -EINVARG;
        goto out;
    }
    if ((uint32_t)phys_end % PAGE_SIZE)
    {
        res = -EINVARG;
        goto out;
    }

    if ((uint32_t)phys_end < (uint32_t)phys)
    {
        res = -EINVARG;
        goto out;
    }

    uint32_t total_bytes = phys_end - phys;
    int total_pages = total_bytes / PAGE_SIZE;
    res = paging_map_range(directory, virt, phys, total_pages, flags);
out:
    return res;
}

int paging_set(uint32_t* directory, void* virt, uint32_t val)
{
    if (!paging_is_aligned(virt))
    {
        return -EINVARG;
    }

    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    int res = paging_get_indexes(virt, &directory_index, &table_index);
    if (res < 0)
    {
        return res;
    }

    uint32_t entry = directory[directory_index];
    uint32_t* table = (uint32_t*)(entry & 0xfffff000);
    table[table_index] = val;

    return 0;
}

void* paging_get_physical_address(uint32_t* directory, void* virt)
{
    void* virt_addr_new = (void*)paging_align_to_lower_page(virt);
    void* difference = (void*)((uint32_t)virt - (uint32_t)virt_addr_new);
    return (void*)((paging_get(directory, virt_addr_new) & 0xfffff000) + difference);
}

uint32_t paging_get(uint32_t* directory, void* virt)
{
    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    paging_get_indexes(virt, &directory_index, &table_index);

    uint32_t entry = directory[directory_index];
    uint32_t* table = (uint32_t*)(entry & 0xfffff000);
    return table[table_index];
}