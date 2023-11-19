#ifndef DISK_H
#define DISK_H

#include "fs/File.h"

typedef unsigned int disk_t;


// Represents a real physical hard disk
#define PEACHOS_DISK_TYPE_REAL 0

struct Disk {
    disk_t type;
    int sector_size;

    // The id of the disk
    int id;

    struct Filesystem *filesystem;

    // The private data of our filesystem
    void *fs_private;
};

extern void disk_search_and_init();

extern struct Disk *disk_get(int index);

extern int disk_read_block(struct Disk *idisk, unsigned int lba, int total, void *buf);

#endif