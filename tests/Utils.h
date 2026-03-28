#ifndef UTILS_H
#define UTILS_H

#include "RandomFile.h"

#include <stdbool.h>

#define SMALL_FILE_SIZE 1 * 1024
#define MEDIUM_FILE_SIZE 500 * 1024
#define LARGE_FILE_SIZE 5 * 1024 * 1024

typedef enum FILESIZE_MODE
{
	SMALL,
	MEDIUM,
	LARGE,
	MIXED
} FILESIZE_MODE;

bool FilesizeModeFromStr(const char *str, FILESIZE_MODE *mode);

RANDOM_FILE **CreateRandomFiles(FILESIZE_MODE mode, size_t num_files);
void DeleteRandomFiles(RANDOM_FILE **files, size_t num_files);

bool CreateHamcore(const char *ham_name, RANDOM_FILE **files, size_t num_files);
void DeleteHamcore(const char *ham_name);

#endif
