#include "Streamer.h"
#include "memory/heap/Kheap.h"
#include "Config.h"

#include <stdbool.h>

struct DiskStream *diskstreamer_new(int disk_id) {
    struct Disk *disk = disk_get(disk_id);
    if (!disk) {
        return 0;
    }

    struct DiskStream *streamer = kzalloc(sizeof(struct DiskStream));
    streamer->pos = 0;
    streamer->disk = disk;
    return streamer;
}

int diskstreamer_seek(struct DiskStream *stream, int pos) {
    stream->pos = pos;
    return 0;
}

int diskstreamer_read(struct DiskStream *stream, void *out, int total) {
    int sector = stream->pos / PEACHOS_SECTOR_SIZE;
    int offset = stream->pos % PEACHOS_SECTOR_SIZE;
    int total_to_read = total;
    bool overflow = (offset + total_to_read) >= PEACHOS_SECTOR_SIZE;
    char buf[PEACHOS_SECTOR_SIZE];

    if (overflow) {
        total_to_read -= (offset + total_to_read) - PEACHOS_SECTOR_SIZE;
    }

    int res = disk_read_block(stream->disk, sector, 1, buf);
    if (res < 0) {
        goto out;
    }


    for (int i = 0; i < total_to_read; i++) {
        *(char *) out++ = buf[offset + i];
    }

    // Adjust the stream
    stream->pos += total_to_read;
    if (overflow) {
        res = diskstreamer_read(stream, out, total - total_to_read);
    }
    out:
    return res;
}

void diskstreamer_close(struct DiskStream *stream) {
    kfree(stream);
}