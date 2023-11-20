#ifndef FILE_H
#define FILE_H

#include "Pparser.h"
#include <stdint.h>

typedef unsigned int file_seek_mode_t;

typedef unsigned int file_mode_t;

typedef unsigned int file_stat_flags_t;

enum {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};


enum {
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

enum {
    FILE_STAT_READ_ONLY = 0b00000001
};

struct Disk;

typedef void *(*FS_OPEN_FUNCTION)(struct Disk *disk, struct PathPart *path, file_mode_t mode);

typedef int (*FS_READ_FUNCTION)(struct Disk *disk, void *

private,
uint32_t size, uint32_t
nmemb,
char *out
);

typedef int (*FS_RESOLVE_FUNCTION)(struct Disk *disk);

typedef int (*FS_CLOSE_FUNCTION)(void *

private);

typedef int (*FS_SEEK_FUNCTION)(void *

private,
uint32_t offset, file_seek_mode_t
seek_mode);


struct FileStat {
    file_stat_flags_t flags;
    uint32_t filesize;
};

typedef int (*FS_STAT_FUNCTION)(struct Disk *disk, void *

private,
struct FileStat *stat
);

struct Filesystem {
    // Filesystem should return zero from resolve if the provided disk is using its filesystem
    FS_RESOLVE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;
    FS_READ_FUNCTION read;
    FS_SEEK_FUNCTION seek;
    FS_STAT_FUNCTION stat;
    FS_CLOSE_FUNCTION close;
    char name[20];
};

struct FileDescriptor {
    // The descriptor index
    int index;

    struct Filesystem *filesystem;

    // Private data for internal file descriptor
    void*
private;

    // The disk that the file descriptor should be used on
    struct Disk *disk;
};


extern void fs_init();

extern int fopen(const char *filename, const char *mode_str);

extern int fseek(int fd, int offset, file_seek_mode_t whence);

extern int fread(void *ptr, uint32_t size, uint32_t nmemb, int fd);

extern int fstat(int fd, struct FileStat *stat);

extern int fclose(int fd);

extern void fs_insert_filesystem(struct Filesystem *filesystem);

extern struct Filesystem *fs_resolve(struct Disk *disk);

#endif