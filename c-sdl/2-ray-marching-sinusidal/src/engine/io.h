#ifndef IO_H
#define IO_H

#include <stdlib.h>
#include <stdbool.h>

#include "types.h"

typedef struct File {
    char* data;
    size_t len;
    bool is_valid;
} File;

File io_file_read(const char *path);
i32 io_file_write(void *buffer, size_t size, const char *path);

#endif