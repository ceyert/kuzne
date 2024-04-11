#ifndef GDT_H
#define GDT_H

#include <stdint.h>

struct Gdt {
    uint16_t segment;
    uint16_t base_first;
    uint8_t base;
    uint8_t access;
    uint8_t high_flags;
    uint8_t base_24_31_bits;
} __attribute__((packed));

enum GdtTypes
{
    GDT_NULL_SEGMENT = 0x00,
    GDT_KERNEL_CODE_SEGMENT = 0x9a,
    GDT_KERNEL_DATA_SEGMENT = 0x92,
    GDT_USER_CODE_SEGMENT = 0xf8,
    GDT_USER_DATA_SEGMENT = 0xf2,
    GDT_TSS_SEGMENT = 0xE9
};

struct GdtStructured {
    uint32_t base;
    uint32_t limit;
    enum GdtTypes type;
};

extern void gdt_load(struct Gdt *gdt, int size);

extern void gdt_structured_to_gdt(struct Gdt *gdt, struct GdtStructured *structured_gdt, int total_entires);

#endif