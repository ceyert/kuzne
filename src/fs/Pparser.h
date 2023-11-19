#ifndef PATHPARSER_H
#define PATHPARSER_H

struct PathRoot {
    int drive_no;
    struct PathPart *first;
};

struct PathPart {
    const char *part;
    struct PathPart *next;
};

struct PathRoot *pathparser_parse(const char *path, const char *current_directory_path);

void pathparser_free(struct PathRoot *root);

#endif