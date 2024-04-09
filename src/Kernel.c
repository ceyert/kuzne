#include "Config.h"
#include "Kernel.h"
#include "Status.h"
#include "disk/Disk.h"
#include "disk/Streamer.h"
#include "fs/File.h"
#include "fs/PathParser.h"
#include "gdt/Gdt.h"
#include "interrupt_descriptor_table/Idt.h"
#include "interrupt_service_routines/interrupt_service_routines.h"
#include "keyboard/Keyboard.h"
#include "memory/Memory.h"
#include "memory/heap/Kheap.h"
#include "memory/paging/Paging.h"
#include "string/String.h"
#include "task/Process.h"
#include "task/Task.h"
#include "task/Tss.h"
#include "vga/Vga.h"

static struct Paging4GbChunk* kernel_chunk = 0;

void kernel_page()
{
    kernel_registers();
    paging_switch(kernel_chunk);
}

struct Tss Tss;

struct Gdt gdt_real[TOTAL_GDT_SEGMENTS];

struct GdtStructured gdt_structured[TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = GDT_NULL_SEGMENT},                 // NULL Segment
    {.base = 0x00, .limit = 0xffffffff, .type = GDT_KERNEL_CODE_SEGMENT},    // Kernel code segment
    {.base = 0x00, .limit = 0xffffffff, .type = GDT_KERNEL_DATA_SEGMENT},    // Kernel data segment
    {.base = 0x00, .limit = 0xffffffff, .type = GDT_USER_CODE_SEGMENT},      // User code segment
    {.base = 0x00, .limit = 0xffffffff, .type = GDT_USER_DATA_SEGMENT},      // User data segment
    {.base = (uint32_t)&Tss, .limit = sizeof(Tss), .type = GDT_TSS_SEGMENT}  // TSS Segment
};

void kernel_main()
{
    terminal_initialize();

    memset(gdt_real, 0x00, sizeof(gdt_real));

    gdt_structured_to_gdt(gdt_real, gdt_structured, TOTAL_GDT_SEGMENTS);

    // Load the gdt
    gdt_load(gdt_real, sizeof(gdt_real));

    // Initialize the heap
    kheap_init();

    // Initialize filesystems
    fs_init();

    // Search and initialize the disks
    disk_search_and_init();

    // Initialize the interrupt descriptor table
    idt_init();

    // Setup the TSS
    memset(&Tss, 0x00, sizeof(Tss));
    Tss.esp0 = 0x600000;
    Tss.ss0 = KERNEL_DATA_SELECTOR;

    // Load the TSS
    tss_load(0x28);

    // Setup paging
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    // Switch to kernel paging chunk
    paging_switch(kernel_chunk);

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

    task_run_first_ever_task();

    while (1)
    {
    }
}