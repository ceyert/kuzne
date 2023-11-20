#ifndef DISKSTREAMER_H
#define DISKSTREAMER_H

#include "Disk.h"

struct DiskStream {
    int position;
    struct Disk *disk;
};

extern struct DiskStream *diskstreamer_new(int disk_id);

extern int diskstreamer_seek(struct DiskStream *stream, int pos);

extern int diskstreamer_read(struct DiskStream *stream, void *out, int total);

extern void diskstreamer_close(struct DiskStream *stream);

#endif