#include "vga/Vga.h"
#include <stdint.h>

#define APIC_BASE_ADDR       0xFEE00000  // Local APIC Base Address
#define APIC_ICR_LOW         0x300       // Offset for ICR Low register
#define APIC_ICR_HIGH        0x310       // Offset for ICR High register
#define APIC_EOI             0xB0        // Offset for End of Interrupt register
#define APIC_SPURIOUS        0xF0        // Offset for Spurious Interrupt register

#define APIC_INIT_IPI        0x500       // Delivery mode for INIT IPI
#define APIC_STARTUP_IPI     0x600       // Delivery mode for STARTUP IPI
#define APIC_DELIVERY_STATUS (1 << 12)   // Delivery status bit

typedef void (*func_ptr_t)(void);

// Shared structures between cores
volatile bool core_ready[4] = {false};      // Status of cores (max 4 cores assumed for simplicity)
volatile func_ptr_t core_tasks[4] = {NULL}; // Task pointers for cores (max 4 cores)

// Example function to execute on a specific core
void example_function() {

    log("Hello \n");
    while (1) {
        // Infinite loop for demonstration
        // Do something specific here
    }
}

// Function to write to an APIC register
static inline void apic_write(uint32_t reg, uint32_t value) {
    *(volatile uint32_t *)(APIC_BASE_ADDR + reg) = value;
}

// Function to read from an APIC register
static inline uint32_t apic_read(uint32_t reg) {
    return *(volatile uint32_t *)(APIC_BASE_ADDR + reg);
}

void apic_send_init_ipi(uint8_t core_id) {
    logAddress("Sending INIT IPI to core %d\n", core_id);
    apic_write(APIC_ICR_HIGH, core_id << 24);
    apic_write(APIC_ICR_LOW, APIC_INIT_IPI);
    while (apic_read(APIC_ICR_LOW) & APIC_DELIVERY_STATUS) {
        log("Waiting for INIT IPI delivery...\n");
    }
    log("INIT IPI delivered.\n");
}

void apic_send_startup_ipi(uint8_t core_id, uint32_t address) {
    // This limits the address range to 0x00000000 to 0x000FF000 (1MB minus 4KB).
    // The address 0x00200000 (2MB) exceeds this range and results in an invalid vector.
    uint32_t vector = (address >> 12) & 0xFF;

    apic_write(APIC_ICR_HIGH, core_id << 24);
    apic_write(APIC_ICR_LOW, APIC_STARTUP_IPI | vector);
    while (apic_read(APIC_ICR_LOW) & APIC_DELIVERY_STATUS) {
        log("Waiting for STARTUP IPI delivery...\n");
    }
    log("STARTUP IPI delivered.\n");
}


// Function to wake a specific core and assign a task
// Function to wake a specific core and assign a task
void wake_core(uint8_t core_id, func_ptr_t func) {
    core_tasks[core_id] = func;       // Assign the function to the specific core
    
    apic_send_init_ipi(core_id);      // Send INIT IPI to reset the core

    // Add a reliable delay using PIT or calibrated loops
    for (volatile int i = 0; i < 1000000; i++); // Hardware-dependent delay

    apic_send_startup_ipi(core_id, 0x00001000); // Send STARTUP IPI (entry point at 0x1000)
}


// Core main loop (executed by APs)
void core_main(uint8_t core_id) {
    core_ready[core_id] = true;  // Mark core as ready
    logAddress("Core %d is ready.\n", core_id);

    while (1) {
        if (core_tasks[core_id]) {
            logAddress("Core %d executing assigned task.\n", core_id);
            core_tasks[core_id]();    // Execute assigned function
            core_tasks[core_id] = NULL;  // Clear task after execution
            logAddress("Core %d completed task.\n", core_id);
        }
    }
}


void __attribute__((section(".core_entry.my_entry"), aligned(4096))) core_entry() {
    log("Core entry point reached.\n");

    uint8_t core_id = (apic_read(0x20) >> 24) & 0xFF;
    logAddress("Core ID determined as %d.\n", core_id);

    core_main(core_id);
}




void setup_secondary_core_entry(struct PageDirectory *kernel_directory, uint32_t new_physical_address) {
    // Ensure the new address is page-aligned
    if (!paging_is_aligned((void *)new_physical_address)) {
        log("Error: Physical address for secondary core is not page-aligned.\n");
        return;
    }

    // Map the physical address to itself (identity mapping)
    int result = map_virtual_address_to_physical_address(
        kernel_directory, 
        (void *)new_physical_address,   // Virtual address
        (void *)new_physical_address,  // Physical address
        PAGING_IS_WRITEABLE | PAGING_IS_PRESENT
    );

    if (result != 0) {
        log("Error: Failed to map secondary core entry point.\n");
    } else {
        logAddress("Secondary core entry mapped at physical address: ", new_physical_address);
    }
}

void start_cores()
{
    // Map the new physical address for the secondary core
   uint32_t secondary_core_address = 0x00004096; // New address for secondary core entry
   setup_secondary_core_entry(KERNEL_PAGE_DIRECTORY_, secondary_core_address);


    // Continue kernel initialization
    apic_write(APIC_SPURIOUS, 0x1FF); // Enable APIC
    wake_core(1, example_function);   // Wake secondary core with the example function
}