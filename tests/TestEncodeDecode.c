#include "FileSystem.h"
#include "Hamcore.h"
#include "RandomFile.h"
#include "Utils.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define HAMCORE_NAME "hamcore.se2"

typedef struct TEST_PARAMS
{
	size_t num_files;
	FILESIZE_MODE mode;
} TEST_PARAMS;

static bool ParseArguments(int argc, char **argv, TEST_PARAMS *params)
{
	if (argc != 3)
	{
		return false;
	}

	if (!FilesizeModeFromStr(argv[1], &params->mode))
	{
		return false;
	}

	int num_files = atoi(argv[2]);
	if (num_files <= 0)
	{
		return false;
	}
	params->num_files = num_files;

	return true;
}

static bool Test(RANDOM_FILE **files, TEST_PARAMS *params)
{
	if (!files || !params)
	{
		return false;
	}

	HAMCORE *hamcore = HamcoreOpen(HAMCORE_NAME);
	if (!hamcore)
	{
		return false;
	}

	// Verify file count matches the number of archived files
	assert(hamcore->Files.Num == params->num_files);

	for (int i = 0; i < params->num_files; i++)
	{
		RANDOM_FILE *random_file = files[i];
		if (!random_file)
		{
			goto CLEANUP;
		}

		const HAMCORE_FILE *hamcore_file = HamcoreFind(hamcore, random_file->Path);
		if (!hamcore_file)
		{
			goto CLEANUP;
		}
		// Verify that the file size after archiving matches the file size before archiving
		assert(hamcore_file->OriginalSize == random_file->Size);

		uint8_t *actual_bytes = malloc(random_file->Size);
		if (!actual_bytes)
		{
			goto CLEANUP;
		}
		FILE *file = Ham_FileOpen(random_file->Path, false);
		if (!file)
		{
			free(actual_bytes);
			goto CLEANUP;
		}
		if (!Ham_FileRead(file, actual_bytes, random_file->Size))
		{
			free(actual_bytes);
			Ham_FileClose(file);
			goto CLEANUP;
		}
		Ham_FileClose(file);

		uint8_t *read_bytes = malloc(hamcore_file->OriginalSize);
		if (!read_bytes)
		{
			free(actual_bytes);
			goto CLEANUP;
		}
		if (!HamcoreRead(hamcore, read_bytes, hamcore_file))
		{
			free(read_bytes);
			free(actual_bytes);
			goto CLEANUP;
		}

		// Verify that the data after archiving matches the archived data before archiving
		assert(memcmp(read_bytes, actual_bytes, random_file->Size) == 0);

		free(read_bytes);
		free(actual_bytes);
	}

	HamcoreClose(hamcore);

	return true;

CLEANUP:
	HamcoreClose(hamcore);
	return false;
}

int main(int argc, char **argv)
{
	TEST_PARAMS parameters = {0};
	if (!ParseArguments(argc, argv, &parameters))
	{
		return 2;
	}

	RANDOM_FILE **random_files = CreateRandomFiles(parameters.mode, parameters.num_files);
	if (!random_files)
	{
		return 1;
	}
	if (!CreateHamcore(HAMCORE_NAME, random_files, parameters.num_files))
	{
		DeleteRandomFiles(random_files, parameters.num_files);
		return 1;
	}

	bool result = Test(random_files, &parameters);

	DeleteHamcore(HAMCORE_NAME);
	DeleteRandomFiles(random_files, parameters.num_files);

	return !result;
}
