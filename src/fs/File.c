#include "File.h"
#include "Config.h"
#include "memory/Memory.h"
#include "memory/heap/Kheap.h"
#include "string/String.h"
#include "disk/Disk.h"
#include "fat/Fat16.h"
#include "Status.h"
#include "Kernel.h"
#include "terminal/Terminal.h"

struct Filesystem *filesystems[PEACHOS_MAX_FILESYSTEMS];
struct FileDescriptor *file_descriptors[PEACHOS_MAX_FILE_DESCRIPTORS];

static struct Filesystem **fs_get_free_filesystem() {
    int i = 0;
    for (i = 0; i < PEACHOS_MAX_FILESYSTEMS; i++) {
        if (filesystems[i] == 0) {
            return &filesystems[i];
        }
    }

    return 0;
}

void fs_insert_filesystem(struct Filesystem *filesystem) {
    struct Filesystem **fs;
    fs = fs_get_free_filesystem();
    if (!fs) {
        print("Problem inserting filesystem");
        while (1) {}
    }

    *fs = filesystem;
}

static void fs_static_load() {
    fs_insert_filesystem(fat16_init());
}

void fs_load() {
    memset(filesystems, 0, sizeof(filesystems));
    fs_static_load();
}

void fs_init() {
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

static void file_free_descriptor(struct FileDescriptor *desc) {
    file_descriptors[desc->index - 1] = 0x00;
    kfree(desc);
}

static int file_new_descriptor(struct FileDescriptor **desc_out) {
    int res = -ENOMEM;
    for (int i = 0; i < PEACHOS_MAX_FILE_DESCRIPTORS; i++) {
        if (file_descriptors[i] == 0) {
            struct FileDescriptor *desc = kzalloc(sizeof(struct FileDescriptor));
            // Descriptors start at 1
            desc->index = i + 1;
            file_descriptors[i] = desc;
            *desc_out = desc;
            res = 0;
            break;
        }
    }

    return res;
}

static struct FileDescriptor *file_get_descriptor(int fd) {
    if (fd <= 0 || fd >= PEACHOS_MAX_FILE_DESCRIPTORS) {
        return 0;
    }

    // Descriptors start at 1
    int index = fd - 1;
    return file_descriptors[index];
}

struct Filesystem *fs_resolve(struct Disk *disk) {
    struct Filesystem *fs = 0;
    for (int i = 0; i < PEACHOS_MAX_FILESYSTEMS; i++) {
        if (filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0) {
            fs = filesystems[i];
            break;
        }
    }

    return fs;
}

FILE_MODE file_get_mode_by_string(const char *str) {
    FILE_MODE mode = FILE_MODE_INVALID;
    if (strncmp(str, "r", 1) == 0) {
        mode = FILE_MODE_READ;
    } else if (strncmp(str, "w", 1) == 0) {
        mode = FILE_MODE_WRITE;
    } else if (strncmp(str, "a", 1) == 0) {
        mode = FILE_MODE_APPEND;
    }
    return mode;
}

int fopen(const char *filename, const char *mode_str) {
    int res = 0;
    struct PathRoot *root_path = pathparser_parse(filename, NULL);
    if (!root_path) {
        res = -EINVARG;
        goto out;
    }

    // We cannot have just a root path 0:/ 0:/test.txt
    if (!root_path->first) {
        res = -EINVARG;
        goto out;
    }

    // Ensure the disk we are reading from exists
    struct Disk *disk = disk_get(root_path->drive_no);
    if (!disk) {
        res = -EIO;
        goto out;
    }

    if (!disk->filesystem) {
        res = -EIO;
        goto out;
    }

    FILE_MODE mode = file_get_mode_by_string(mode_str);
    if (mode == FILE_MODE_INVALID) {
        res = -EINVARG;
        goto out;
    }

    void *descriptor_private_data = disk->filesystem->open(disk, root_path->first, mode);
    if (ISERR(descriptor_private_data)) {
        res = ERROR_I(descriptor_private_data);
        goto out;
    }

    struct FileDescriptor *desc = 0;
    res = file_new_descriptor(&desc);
    if (res < 0) {
        goto out;
    }
    desc->filesystem = disk->filesystem;
    desc->private = descriptor_private_data;
    desc->disk = disk;
    res = desc->index;

    out:
    // fopen shouldnt return negative values
    if (res < 0)
        res = 0;

    return res;
}

int fstat(int fd, struct FileStat *stat) {
    int res = 0;
    struct FileDescriptor *desc = file_get_descriptor(fd);
    if (!desc) {
        res = -EIO;
        goto out;
    }

    res = desc->filesystem->stat(desc->disk, desc->private, stat);
    out:
    return res;
}

int fclose(int fd) {
    int res = 0;
    struct FileDescriptor *desc = file_get_descriptor(fd);
    if (!desc) {
        res = -EIO;
        goto out;
    }

    res = desc->filesystem->close(desc->private);
    if (res == PEACHOS_ALL_OK) {
        file_free_descriptor(desc);
    }
    out:
    return res;
}

int fseek(int fd, int offset, FILE_SEEK_MODE whence) {
    int res = 0;
    struct FileDescriptor *desc = file_get_descriptor(fd);
    if (!desc) {
        res = -EIO;
        goto out;
    }

    res = desc->filesystem->seek(desc->private, offset, whence);
    out:
    return res;
}

int fread(void *ptr, uint32_t size, uint32_t nmemb, int fd) {
    int res = 0;
    if (size == 0 || nmemb == 0 || fd < 1) {
        res = -EINVARG;
        goto out;
    }

    struct FileDescriptor *desc = file_get_descriptor(fd);
    if (!desc) {
        res = -EINVARG;
        goto out;
    }

    res = desc->filesystem->read(desc->disk, desc->private, size, nmemb, (char *) ptr);
    out:
    return res;
}