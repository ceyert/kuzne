#include "Config.h"
#include "Kernel.h"
#include "Status.h"
#include "disk/Disk.h"
#include "disk/Streamer.h"
#include "fs/File.h"
#include "fs/PathParser.h"
#include "global_descriptor_table/Gdt.h"
#include "interrupt_descriptor_table/Idt.h"
#include "interrupt_service_routines/interrupt_service_routines.h"
#include "io/Io.h"
#include "keyboard/Keyboard.h"
#include "malloc/Kheap.h"
#include "memory/Memory.h"
#include "paging/Paging.h"
#include "process/Process.h"
#include "process/Task.h"
#include "process/Tss.h"
#include "vga/Vga.h"

/*
https://wiki.osdev.org/Detecting_Memory_(x86)

Disables IRQs to prevent interruptions during the memory probing process.
It also saves the current state of the CR0 register and invalidates the cache to ensure accurate memory testing.

This function enters a loop where it increments the memory address by 1MB steps.
For each step, it writes a test pattern (0x55AA55AA) to the memory location and verifies it.
If the memory location can store and return the correct value, it increments the memory count.
If not, it assumes the end of available memory has been reached.
*/
static void probe_8gb_memory_count(void)
{
    uint32_t* mem;
    uint32_t mem_count = 0, a;
    uint16_t memkb = 0;
    uint8_t irq1, irq2;
    uint32_t cr0;

    // Save IRQs
    irq1 = insb(0x21);
    irq2 = insb(0xA1);

    // Disable all IRQs
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);

    // Store a copy of CR0
    __asm__ __volatile__("movl %%cr0, %%eax" : "=a"(cr0));

    // Invalidate the cache
    __asm__ __volatile__("wbinvd");

    // Set CR0 to disable cache and enable protected mode
    __asm__ __volatile__("movl %%eax, %%cr0" ::"a"(cr0 | 0x00000001 | 0x40000000 | 0x20000000));

    do
    {
        memkb++;
        mem_count += 1024 * 1024;
        mem = (uint32_t*)mem_count;

        a = *mem;
        *mem = 0x55AA55AA;
        __asm__ __volatile__("" ::: "memory");

        if (*mem != 0x55AA55AA)
            mem_count = 0;
        else
        {
            *mem = 0xAA55AA55;
            __asm__ __volatile__("" ::: "memory");
            if (*mem != 0xAA55AA55) mem_count = 0;
        }

        __asm__ __volatile__("" ::: "memory");
        *mem = a;

    } while (memkb < 8192 && mem_count != 0);  // 8192 MB => 8GB

    // Restore CR0
    __asm__ __volatile__("movl %%eax, %%cr0" ::"a"(cr0));

    uint32_t mem_end = memkb << 20;

    // Restore IRQs
    outb(0x21, irq1);
    outb(0xA1, irq2);

    logAddress("total memory in bytes: ", mem_end);  // 0x20000000

    // 512MB of RAM, the addresses range is from 0x00000000 to 0x1FFFFFFF
    logAddress("End memory address: ", mem_end);  // 0x20000000
}

static struct PageDirectory* KERNEL_PAGE_DIRECTORY_ = 0;

void kernel_page()
{
    kernel_registers();
    set_current_page_directory(KERNEL_PAGE_DIRECTORY_);
}

struct Tss TSS_;

struct Gdt GDT_REAL_[TOTAL_GDT_SEGMENTS];

struct GdtStructured GDT_STRUCTURED_[TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = GDT_NULL_SEGMENT},  // NULL Segment

    {.base = 0x00, .limit = 0xffffffff, .type = GDT_KERNEL_CODE_SEGMENT},  // Kernel code segment

    {.base = 0x00, .limit = 0xffffffff, .type = GDT_KERNEL_DATA_SEGMENT},  // Kernel data segment

    {.base = 0x00, .limit = 0xffffffff, .type = GDT_USER_CODE_SEGMENT},  // User code segment

    {.base = 0x00, .limit = 0xffffffff, .type = GDT_USER_DATA_SEGMENT},  // User data segment

    {.base = (uint32_t)&TSS_, .limit = sizeof(TSS_), .type = GDT_TSS_SEGMENT}  // TSS Segment
};

void kernel_main()
{
    terminal_initialize();

    memset(GDT_REAL_, 0x00, sizeof(GDT_REAL_));

    gdt_structured_to_gdt(GDT_REAL_, GDT_STRUCTURED_, TOTAL_GDT_SEGMENTS);

    // Load the gdt
    gdt_load(GDT_REAL_, sizeof(GDT_REAL_));

    // Initialize the heap
    kernel_heap_init();

    // Initialize filesystems
    fs_init();

    // Search and initialize the disks
    disk_search_and_init();

    // Initialize the interrupt descriptor table
    idt_init();

    // Setup the TSS
    memset(&TSS_, 0x00, sizeof(TSS_));
    TSS_.esp0 = 0x600000;
    TSS_.ss0 = KERNEL_DATA_SELECTOR;

    // Load the TSS
    tss_load(0x28);

    // allocate page directory for the kernel
    KERNEL_PAGE_DIRECTORY_ =
        new_page_directory_allocation(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    // Switch to kernel paging chunk
    set_current_page_directory(KERNEL_PAGE_DIRECTORY_);

    // Enable kernel paging
    enable_paging();

    // Register the kernel commands
    isr80h_register_commands();

    // Initialize all the system keyboards
    keyboard_init();

    probe_8gb_memory_count();

    struct Process* process = 0;
    int res = process_load_switch("0:/shell.elf", &process);
    if (res != ALL_OK)
    {
        panic("Failed to load shell.elf\n");
    }

    run_first_task();

    while (1)
    {
    }
}