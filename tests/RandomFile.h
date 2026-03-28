#ifndef RANDOMFILE_H
#define RANDOMFILE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct RANDOM_FILE
{
	size_t Size;
	char *Path;
} RANDOM_FILE;

RANDOM_FILE *CreateRandomFile(const char *path, const size_t size);
void DeleteRandomFile(RANDOM_FILE *file);

#endif
