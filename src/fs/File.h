/**
 * @file File.h
 * @brief File system interface and operations.
 *
 * Defines the interface and operations for file management on a disk, including opening,
 * reading, seeking, and closing files. It also includes definitions for file system registration
 * and resolution based on disk content.
 */

#ifndef FILE_H
#define FILE_H

#include "PathParser.h"
#include <stdint.h>

typedef unsigned int file_seek_mode_t; ///< Seek mode for file operations.
typedef unsigned int file_mode_t; ///< Mode for opening files.
typedef unsigned int file_stat_flags_t; ///< Flags for file status.

/**
 * Seek modes for fseek.
 */
enum {
    SEEK_SET, ///< Seek from the beginning of the file.
    SEEK_CUR, ///< Seek from the current file position.
    SEEK_END  ///< Seek from the end of the file.
};

/**
 * Modes for opening files.
 */
enum {
    FILE_MODE_READ,   ///< Open file for reading.
    FILE_MODE_WRITE,  ///< Open file for writing.
    FILE_MODE_APPEND, ///< Open file for appending.
    FILE_MODE_INVALID ///< Invalid mode, used for error checking.
};

/**
 * Flags for file status.
 */
enum {
    FILE_STAT_READ_ONLY = 0b00000001 ///< File is read-only.
};

struct Disk; ///< Forward declaration for Disk structure.

/**
 * Function type for opening files.
 */
typedef void *(*FS_OPEN_FUNCTION)(struct Disk *disk, struct PathPart *path, file_mode_t mode);

/**
 * Function type for reading from files.
 */
typedef int (*FS_READ_FUNCTION)(struct Disk *disk, void *private, uint32_t size, uint32_t nmemb, char *out);

/**
 * Function type for resolving file system type on a disk.
 */
typedef int (*FS_RESOLVE_FUNCTION)(struct Disk *disk);

/**
 * Function type for closing files.
 */
typedef int (*FS_CLOSE_FUNCTION)(void *private);

/**
 * Function type for seeking within files.
 */
typedef int (*FS_SEEK_FUNCTION)(void *private, uint32_t offset, file_seek_mode_t seek_mode);

/**
 * Structure representing file statistics.
 */
struct FileStat {
    file_stat_flags_t flags; ///< Flags representing file status.
    uint32_t filesize;       ///< Size of the file.
};

/**
 * Function type for retrieving file statistics.
 */
typedef int (*FS_STAT_FUNCTION)(struct Disk *disk, void *private, struct FileStat *stat);

/**
 * Represents a file system with operations.
 */
struct Filesystem {
    FS_RESOLVE_FUNCTION resolve; ///< Function to resolve file system.
    FS_OPEN_FUNCTION open;       ///< Function to open files.
    FS_READ_FUNCTION read;       ///< Function to read files.
    FS_SEEK_FUNCTION seek;       ///< Function to seek within files.
    FS_STAT_FUNCTION stat;       ///< Function to get file statistics.
    FS_CLOSE_FUNCTION close;     ///< Function to close files.
    char name[20];               ///< Name of the file system.
};

/**
 * Represents a file descriptor with associated operations and data.
 */
struct FileDescriptor {
    int index;                  ///< Descriptor index.
    struct Filesystem *filesystem; ///< Associated file system.
    void *private;              ///< Private data for file descriptor.
    struct Disk *disk;          ///< Disk associated with the file.
};

extern void fs_init(); ///< Initializes the file system module.

extern int fopen(const char *filename, const char *mode_str); ///< Opens a file.

extern int fseek(int fd, int offset, file_seek_mode_t whence); ///< Seeks within a file.

extern int fread(void *ptr, uint32_t size, uint32_t nmemb, int fd); ///< Reads from a file.

extern int fstat(int fd, struct FileStat *stat); ///< Retrieves file statistics.

extern int fclose(int fd); ///< Closes a file.

extern void fs_insert_filesystem(struct Filesystem *filesystem); ///< Registers a new file system.

extern struct Filesystem *fs_resolve(struct Disk *disk); ///< Resolves the file system for a disk.

#endif // FILE_H