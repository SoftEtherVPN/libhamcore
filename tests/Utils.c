#include "Utils.h"
#include "Hamcore.h"
#include "RandomFile.h"

#include <stdlib.h>
#include <string.h>

bool FilesizeModeFromStr(const char *str, FILESIZE_MODE *mode)
{
	if (!str)
	{
		return false;
	}

	if (strcmp(str, "SMALL") == 0)
	{
		*mode = SMALL;
	}
	else if (strcmp(str, "MEDIUM") == 0)
	{
		*mode = MEDIUM;
	}
	else if (strcmp(str, "LARGE") == 0)
	{
		*mode = LARGE;
	}
	else if (strcmp(str, "MIXED") == 0)
	{
		*mode = MIXED;
	}
	else
	{
		return false;
	}

	return true;
}

static bool CreateSequenceFile(const char *format, RANDOM_FILE **files, size_t size, size_t num_files)
{
	if (!format || !files)
	{
		return false;
	}

	char path[64] = {0};
	for (int i = 0; i < num_files; i++)
	{
		if (snprintf(path, sizeof(path), format, i) < 0)
		{
			return false;
		}
		files[i] = CreateRandomFile(path, size);
		if (!files[i])
		{
			return false;
		}
	}

	return true;
}

RANDOM_FILE **CreateRandomFiles(FILESIZE_MODE mode, size_t num_files)
{
	if (num_files == 0)
	{
		return NULL;
	}

	RANDOM_FILE **files = malloc(sizeof(RANDOM_FILE *) * num_files);
	if (!files)
	{
		return NULL;
	}
	memset(files, 0, sizeof(RANDOM_FILE *) * num_files);

	size_t num_small_files = 0;
	size_t num_medium_files = 0;
	size_t num_large_files = 0;
	if (mode == SMALL)
	{
		num_small_files = num_files;
	}
	else if (mode == MEDIUM)
	{
		num_medium_files = num_files;
	}
	else if (mode == LARGE)
	{
		num_large_files = num_files;
	}
	else if (mode == MIXED)
	{
		size_t part = num_files / 3;
		num_small_files = part + (num_files % 3);
		num_medium_files = part;
		num_large_files = part;
	}

	size_t total = 0;
	if (!CreateSequenceFile("small%09d", &files[total], SMALL_FILE_SIZE, num_small_files))
	{
		goto FINAL;
	}
	total += num_small_files;

	if (!CreateSequenceFile("medium%09d", &files[total], MEDIUM_FILE_SIZE, num_medium_files))
	{
		goto FINAL;
	}
	total += num_medium_files;

	if (!CreateSequenceFile("large%09d", &files[total], LARGE_FILE_SIZE, num_large_files))
	{
		goto FINAL;
	}
	total += num_large_files;

	return files;

FINAL:
	DeleteRandomFiles(files, num_files);
	return NULL;
}

void DeleteRandomFiles(RANDOM_FILE **files, size_t num_files)
{
	if (!files || num_files == 0)
	{
		return;
	}

	for (int i = 0; i < num_files; i++)
	{
		DeleteRandomFile(files[i]);
	}

	free(files);
}

bool CreateHamcore(const char *ham_name, RANDOM_FILE **files, size_t num_files)
{
	if (!ham_name || !files || num_files == 0)
	{
		return false;
	}

	const char **src_paths = malloc(sizeof(char *) * num_files);
	if (!src_paths)
	{
		return false;
	}
	for (int i = 0; i < num_files; i++)
	{
		src_paths[i] = files[i]->Path;
	}

	bool result = HamcoreBuild(ham_name, NULL, src_paths, num_files);

	free(src_paths);

	return result;
}

void DeleteHamcore(const char *ham_name)
{
	remove(ham_name);
}
