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

struct GdtStructured {
    uint32_t base;
    uint32_t limit;
    uint8_t type;
};

extern void gdt_load(struct Gdt *gdt, int size);

extern void gdt_structured_to_gdt(struct Gdt *gdt, struct GdtStructured *structured_gdt, int total_entires);

#endif