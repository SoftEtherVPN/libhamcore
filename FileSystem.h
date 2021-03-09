#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdbool.h>
#include <stdio.h>

FILE *FileOpen(const char *path, const bool write);
bool FileClose(FILE *file);

bool FileRead(FILE *file, void *dst, const size_t size);
bool FileWrite(FILE *file, const void *src, const size_t size);

bool FileSeek(FILE *file, const size_t offset);

size_t FileSize(const char *path);

const char *PathRelativeToBase(const char *full, const char *base);

#endif
