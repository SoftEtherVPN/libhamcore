#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdbool.h>
#include <stdio.h>

FILE *FileOpen(const char *path);
bool FileClose(FILE *file);

bool FileRead(FILE *file, void *dst, const size_t size);
bool FileSeek(FILE *file, const size_t offset);

#endif
