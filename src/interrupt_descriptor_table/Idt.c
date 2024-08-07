#include "Config.h"
#include "Idt.h"
#include "Kernel.h"
#include "Status.h"
#include "io/Io.h"
#include "memory/Memory.h"
#include "process/Process.h"
#include "process/Task.h"
#include "vga/Vga.h"

struct IdtDesc idt_descriptors[TOTAL_INTERRUPTS];
struct IdtrDesc idtr_descriptor;

extern void* interrupt_pointer_table[TOTAL_INTERRUPTS];

static INTERRUPT_CALLBACK_FUNCTION interrupt_callbacks[TOTAL_INTERRUPTS];

static ISR80H_COMMAND isr80h_commands[MAX_ISR80H_COMMANDS];

extern void idt_load(struct IdtrDesc* ptr);

extern void int21h();

extern void no_interrupt();

extern void isr80h_wrapper();

void no_interrupt_handler()
{
    outb(0x20, 0x20);
}

void idt_zero_callback()
{
    panic("Divide by zero error!\n");
}


void idt_page_fault_callback()
{
    panic("Page fault error!\n");
}

void interrupt_handler(int interrupt, struct InterruptFrame* frame)
{
    kernel_page(); // switch to kernel pages

    if (interrupt_callbacks[interrupt] != 0)
    {
        task_current_save_state(frame);
        interrupt_callbacks[interrupt](frame);
    }

    task_page(); // switch to process pages
    outb(0x20, 0x20);
}


void idt_set(int interrupt_no, void* address)
{
    struct IdtDesc* desc = &idt_descriptors[interrupt_no];
    desc->offset_1 = (uint32_t)address & 0x0000ffff;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = 0xEE;
    desc->offset_2 = (uint32_t)address >> 16;
}

void idt_handle_exception()
{
    process_terminate(task_current()->process);
    run_next_task();
}

// Used to schedule tasks
void idt_clock()
{
    outb(0x20, 0x20);

    // Switch to the next task
    run_next_task();
}

void idt_init()
{
/*
    IVT Offset | INT #     | Description
-----------+-----------+-----------------------------------
0x0000     | 0x00      | Divide by 0
0x0004     | 0x01      | Reserved
0x0008     | 0x02      | NMI Interrupt
0x000C     | 0x03      | Breakpoint (INT3)
0x0010     | 0x04      | Overflow (INTO)
0x0014     | 0x05      | Bounds range exceeded (BOUND)
0x0018     | 0x06      | Invalid opcode (UD2)
0x001C     | 0x07      | Device not available (WAIT/FWAIT)
0x0020     | 0x08      | Double fault
0x0024     | 0x09      | Coprocessor segment overrun
0x0028     | 0x0A      | Invalid TSS
0x002C     | 0x0B      | Segment not present
0x0030     | 0x0C      | Stack-segment fault
0x0034     | 0x0D      | General protection fault
0x0038     | 0x0E      | Page fault
0x003C     | 0x0F      | Reserved
0x0040     | 0x10      | x87 FPU error
0x0044     | 0x11      | Alignment check
0x0048     | 0x12      | Machine check
0x004C     | 0x13      | SIMD Floating-Point Exception
0x00xx     | 0x14-0x1F | Reserved
0x0xxx     | 0x20-0xFF | User definable
*/

    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = (uint32_t)idt_descriptors;

    for (int i = 0; i < TOTAL_INTERRUPTS; i++)
    {
        idt_set(i, interrupt_pointer_table[i]);
    }

    // https://wiki.osdev.org/Exceptions
    idt_set(0, idt_zero_callback);
    idt_set(0x80, isr80h_wrapper); // set interrupt service routines
    idt_set(0xE, idt_page_fault_callback);

    for (int i = 0; i < 0x20; i++)
    {
        idt_register_interrupt_callback(i, idt_handle_exception);
    }

    // Clock interrupt
    int clock_int = 0x20;
    idt_register_interrupt_callback(clock_int, idt_clock);

    // Load the interrupt descriptor table
    idt_load(&idtr_descriptor);
}

int idt_register_interrupt_callback(int interrupt, INTERRUPT_CALLBACK_FUNCTION interrupt_callback)
{
    if (interrupt < 0 || interrupt >= TOTAL_INTERRUPTS)
    {
        return -EINVARG;
    }

    interrupt_callbacks[interrupt] = interrupt_callback;
    return 0;
}

void isr80h_register_command(int command_id, ISR80H_COMMAND command)
{
    if (command_id < 0 || command_id >= MAX_ISR80H_COMMANDS)
    {
        panic("The command is out of bounds\n");
    }

    if (isr80h_commands[command_id])
    {
        panic("Your attempting to overwrite an existing command\n");
    }

    isr80h_commands[command_id] = command;
}

void* isr80h_handle_command(int command, struct InterruptFrame* frame)
{
    void* result = 0;

    if (command < 0 || command >= MAX_ISR80H_COMMANDS)
    {
        // Invalid command
        return 0;
    }

    ISR80H_COMMAND command_func = isr80h_commands[command];
    if (!command_func)
    {
        return 0;
    }

    result = command_func(frame);
    return result;
}

// Executes interrupt service routine called by 'isr80h_wrapper' 
void* isr80h_handler(int command, struct InterruptFrame* frame)
{
    void* res = 0;
    kernel_page();
    task_current_save_state(frame);
    res = isr80h_handle_command(command, frame);
    task_page();
    return res;
}