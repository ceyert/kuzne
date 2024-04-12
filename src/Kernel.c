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
#include "keyboard/Keyboard.h"
#include "memory/Memory.h"
#include "memory/heap/Kheap.h"
#include "paging/Paging.h"
#include "string/String.h"
#include "process/Process.h"
#include "process/Task.h"
#include "process/Tss.h"
#include "vga/Vga.h"

static struct Paging4GbChunk* KERNEL_PAGE_DIRECTORY_ = 0;

void kernel_page()
{
    kernel_registers();
    set_current_page_directory(KERNEL_PAGE_DIRECTORY_);
}

struct Tss TSS_;

struct Gdt GDT_REAL_[TOTAL_GDT_SEGMENTS];

struct GdtStructured GDT_STRUCTURED_[TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = GDT_NULL_SEGMENT},                 // NULL Segment

    {.base = 0x00, .limit = 0xffffffff, .type = GDT_KERNEL_CODE_SEGMENT},    // Kernel code segment

    {.base = 0x00, .limit = 0xffffffff, .type = GDT_KERNEL_DATA_SEGMENT},    // Kernel data segment

    {.base = 0x00, .limit = 0xffffffff, .type = GDT_USER_CODE_SEGMENT},      // User code segment

    {.base = 0x00, .limit = 0xffffffff, .type = GDT_USER_DATA_SEGMENT},      // User data segment

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

    // Setup paging
    KERNEL_PAGE_DIRECTORY_ = enable_4gb_virtual_memory_addressing(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    // Switch to kernel paging chunk
    set_current_page_directory(KERNEL_PAGE_DIRECTORY_);

    // Enable paging
    enable_paging();

    // Register the kernel commands
    isr80h_register_commands();

    // Initialize all the system keyboards
    keyboard_init();

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