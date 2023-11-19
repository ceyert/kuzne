#ifndef DISKSTREAMER_H
#define DISKSTREAMER_H

#include "Disk.h"

struct DiskStream {
    int pos;
    struct Disk *disk;
};

struct DiskStream *diskstreamer_new(int disk_id);

int diskstreamer_seek(struct DiskStream *stream, int pos);

int diskstreamer_read(struct DiskStream *stream, void *out, int total);

void diskstreamer_close(struct DiskStream *stream);

#endif