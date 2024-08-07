#include "Kernel.h"
#include "PathParser.h"
#include "Status.h"
#include "memory/Memory.h"
#include "malloc/Kheap.h"

static int pathparser_path_valid_format(const char* filename)
{
    int len = strnlen(filename, PEACHOS_MAX_PATH);
    return (len >= 3 && isdigit(filename[0]) && memcmp((void*)&filename[1], ":/", 2) == 0);
}

static int pathparser_get_drive_by_path(const char** path)
{
    if (!pathparser_path_valid_format(*path))
    {
        return -EBADPATH;
    }

    int drive_no = tonumericdigit(*path[0]);

    // Add 3 bytes to skip drive number 0:/ 1:/ 2:/
    *path += 3;
    return drive_no;
}

static struct PathRoot* pathparser_create_root(int drive_number)
{
    struct PathRoot* path_r = kernel_zeroed_alloc(sizeof(struct PathRoot));
    path_r->driveNo = drive_number;
    path_r->first = 0;
    return path_r;
}

static const char* pathparser_get_path_part(const char** path)
{
    char* result_path_part = kernel_zeroed_alloc(PEACHOS_MAX_PATH);
    int i = 0;
    while (**path != '/' && **path != 0x00)
    {
        result_path_part[i] = **path;
        *path += 1;
        i++;
    }

    if (**path == '/')
    {
        // Skip the forward slash to avoid problems
        *path += 1;
    }

    if (i == 0)
    {
        kernel_free_alloc(result_path_part);
        result_path_part = 0;
    }

    return result_path_part;
}

struct PathPart* pathparser_parse_path_part(struct PathPart* last_part, const char** path)
{
    const char* path_part_str = pathparser_get_path_part(path);
    if (!path_part_str)
    {
        return 0;
    }

    struct PathPart* part = kernel_zeroed_alloc(sizeof(struct PathPart));
    part->part = path_part_str;
    part->next = 0x00;

    if (last_part)
    {
        last_part->next = part;
    }

    return part;
}

void pathparser_free(struct PathRoot* root)
{
    struct PathPart* part = root->first;
    while (part)
    {
        struct PathPart* next_part = part->next;
        kernel_free_alloc((void*)part->part);
        kernel_free_alloc(part);
        part = next_part;
    }

    kernel_free_alloc(root);
}

struct PathRoot* pathparser_parse(const char* path, const char* current_directory_path)
{
    int res = 0;
    const char* tmp_path = path;
    struct PathRoot* path_root = 0;

    if (strlen(path) > PEACHOS_MAX_PATH)
    {
        goto out;
    }

    res = pathparser_get_drive_by_path(&tmp_path);
    if (res < 0)
    {
        goto out;
    }

    path_root = pathparser_create_root(res);
    if (!path_root)
    {
        goto out;
    }

    struct PathPart* first_part = pathparser_parse_path_part(NULL, &tmp_path);
    if (!first_part)
    {
        goto out;
    }

    path_root->first = first_part;
    struct PathPart* part = pathparser_parse_path_part(first_part, &tmp_path);
    while (part)
    {
        part = pathparser_parse_path_part(part, &tmp_path);
    }

out:
    return path_root;
}