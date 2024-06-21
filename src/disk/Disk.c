#include "Config.h"
#include "Disk.h"
#include "Status.h"
#include "io/Io.h"
#include "memory/Memory.h"

struct Disk Disk;

int disk_read_sector(int lba, int total, void* buf)
{
    outb(0x1F6, (lba >> 24) | 0xE0);
    outb(0x1F2, total);
    outb(0x1F3, (unsigned char)(lba & 0xff));
    outb(0x1F4, (unsigned char)(lba >> 8));
    outb(0x1F5, (unsigned char)(lba >> 16));
    outb(0x1F7, 0x20);

    unsigned short* ptr = (unsigned short*)buf;
    for (int b = 0; b < total; b++)
    {
        // Wait for the buffer to be ready
        char c = insb(0x1F7);
        while (!(c & 0x08))
        {
            c = insb(0x1F7);
        }

        // Copy from hard disk to memory
        for (int i = 0; i < 256; i++)
        {
            *ptr = insw(0x1F0);
            ptr++;
        }
    }
    return 0;
}

void disk_search_and_init()
{
    memset(&Disk, 0, sizeof(Disk));
    Disk.type = REAL_HARD_DISK_TYPE;
    Disk.sectorSize = SECTOR_SIZE;
    Disk.diskId = 0;
    Disk.filesystem = fs_resolve(&Disk);
}

struct Disk* disk_get(int index)
{
    if (index != 0) return 0;

    return &Disk;
}

int disk_read_block(struct Disk* idisk, unsigned int lba, int total, void* buf)
{
    if (idisk != &Disk)
    {
        return -EIO;
    }

    return disk_read_sector(lba, total, buf);
}