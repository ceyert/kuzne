#ifndef DISK_H
#define DISK_H

#include "fs/File.h"

typedef unsigned int disk_t;

// Represents a real physical hard disk
#define REAL_HARD_DISK_TYPE 0

struct Disk {

    disk_t type;

    int sectorSize;

    // The id of the disk
    int diskId;

    struct Filesystem *filesystem;

    // The private data of our filesystem
    void *fsPrivate;
};

extern void disk_search_and_init();

extern struct Disk *disk_get(int index);

extern int disk_read_block(struct Disk *idisk, unsigned int lba, int total, void *buf);

#endif