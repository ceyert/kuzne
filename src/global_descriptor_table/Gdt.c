/**
 * @file GdtEncoder.c
 * @brief Encodes GDT entries from structured representations to raw byte format.
 *
 * This file contains the functions necessary to encode Global Descriptor Table (GDT) entries
 * from a high-level, structured representation into the raw byte format required by the CPU.
 * It includes functionality to handle large segment limits by switching to granularity mode
 * and ensures that the base address and segment limits are correctly encoded into the GDT entry.
 */

#include "Gdt.h"
#include "vga/Vga.h"

/**
 * @brief Encodes a GDT entry from a structured representation into raw bytes.
 *
 * This function encodes a single Global Descriptor Table (GDT) entry from a structured
 * representation (GdtStructured) into a raw byte format suitable for use by the processor.
 * It handles the conversion of limit and base address fields and sets the appropriate flags
 * according to the granularity and segment limit.
 *
 * @param target A pointer to an array of bytes where the encoded GDT entry will be stored.
 * @param source A GdtStructured object containing the high-level representation of the GDT entry.
 */
void encodeGdtEntry(uint8_t* target, struct GdtStructured source)
{
    // Check for limit to ensure it's within the valid range, adjust if necessary
    if ((source.limit > 65536) && ((source.limit & 0xFFF) != 0xFFF))
    {
        panic("encodeGdtEntry: Invalid argument\n");
    }

    // Set granularity if limit is higher than 20 bits
    target[6] = 0x40;
    if (source.limit > 65536)
    {
        // Adjust limit for granularity and set granularity flag
        source.limit = source.limit >> 12;
        target[6] = 0xC0;
    }

    // Encode the limit into the target
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] |= (source.limit >> 16) & 0x0F;

    // Encode the base into the target
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;

    // Set the type field
    target[5] = source.type;
}

/**
 * @brief Converts an array of structured GDT entries to raw format.
 *
 * This function iterates over an array of structured GDT entries (GdtStructured) and
 * converts each entry into its raw byte representation. It utilizes encodeGdtEntry()
 * to perform the conversion for each entry.
 *
 * @param gdt A pointer to the beginning of an array of GDT entries (in raw format) where the
 *            encoded entries will be stored.
 * @param structured_gdt A pointer to the beginning of an array of GdtStructured entries that
 *                       are to be encoded.
 * @param total_entires The total number of entries in the structured_gdt array to be converted.
 */
void gdt_structured_to_gdt(struct Gdt* gdt, struct GdtStructured* structured_gdt, int total_entires)
{
    for (int i = 0; i < total_entires; i++)
    {
        encodeGdtEntry((uint8_t*)&gdt[i], structured_gdt[i]);
    }
}
