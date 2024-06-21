# GDT

The Global Descriptor Table (GDT) is used to isolate and protect different areas of memory. The GDT is part of the legacy segmentation model but is still essential in modern x86 systems, including those operating in protected mode and long mode, to define the basic memory segments and to facilitate certain operations such as task switching.

### Structure of the GDT

Each entry in the GDT is an 8-byte descriptor that defines a memory segment's base address, limit (size), and access permissions (such as read/write and execute permissions). These descriptors can represent various types of segments, including:

- **Code Segments**: Areas of memory containing executable code, with permissions that control execution and reading.
- **Data Segments**: Areas of memory for storing data, with permissions that control reading and writing.
- **Stack Segments**: Special data segments used for the stack, with permissions tailored for stack operations.
- **System Segments and Gates**: Used for tasks like task state segments (TSS), call gates, and interrupt gates, which are essential for task switching, system calls, and interrupt handling.

### Purpose and Functionality

- **Memory Protection**: By defining segments with specific access rights, the GDT helps isolate different parts of the program and protect memory. For example, it can prevent a data segment from being executed or a code segment from being written to.
- **Address Translation**: Segmentation allows the operating system to use logical addresses (segment:offset pairs) that are translated into linear addresses through the segment descriptors in the GDT. This adds a layer of abstraction that can enhance security and flexibility.
- **Control Flow and Privilege Levels**: The GDT helps manage control flow between different privilege levels (rings). For example, it can enforce that certain operations or accesses to specific segments are only allowed from higher privilege levels, supporting the implementation of protected and user modes.

### Usage in Modern x86 Systems

Although segmentation was originally a fundamental part of memory management in x86 systems, modern x86 operating systems primarily rely on paging for memory management.

- **Defining a Flat Memory Model**: Many operating systems use the GDT to set up a minimal set of segments that essentially span the entire address space (a flat memory model), effectively bypassing segmentation but still using the GDT as required by the processor.
- **System Tasks and Privilege Levels**: The GDT is used to define segments for system-level tasks, handling interrupts, and managing privilege levels, even in systems that mainly use paging.
