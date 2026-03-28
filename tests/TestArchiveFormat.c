#include "FileSystem.h"
#include "Hamcore.h"
#include "Memory.h"
#include "RandomFile.h"
#include "Utils.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

#define HAMCORE_NAME "hamcore.se2"

typedef struct TEST_PARAMS
{
	size_t num_files;
	FILESIZE_MODE mode;
} TEST_PARAMS;

static bool ParseArguments(int argc, char **argv, TEST_PARAMS *parameter)
{
	if (argc != 3)
	{
		return false;
	}

	if (!FilesizeModeFromStr(argv[1], &parameter->mode))
	{
		return false;
	}

	int num_files = atoi(argv[2]);
	if (num_files <= 0)
	{
		return false;
	}
	parameter->num_files = num_files;

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

	FILE *ham_file = hamcore->File;
	if (!Ham_FileSeek(ham_file, 0))
	{
		goto FINAL;
	}

	// Verify magic header matches "HamCore"
	uint8_t header[7] = {};
	if (!Ham_FileRead(ham_file, header, sizeof(header)))
	{
		goto FINAL;
	}
	assert(memcmp(header, HAMCORE_HEADER_DATA, 7) == 0);

	// Verify file count matches the number of archived files
	uint32_t num_files = 0;
	if (!Ham_FileRead(ham_file, &num_files, sizeof(uint32_t)))
	{
		goto FINAL;
	}
	assert(BigEndian32(num_files) == params->num_files);

	// Check file table
	for (int i = 0; i < params->num_files; i++)
	{
		RANDOM_FILE *file = files[i];
		if (!file)
		{
			goto FINAL;
		}
		uint32_t path_actual_len = strlen(file->Path);

		// Verify path length matches (stored as strlen + 1)
		uint32_t path_len;
		if (!Ham_FileRead(ham_file, &path_len, sizeof(uint32_t)))
		{
			goto FINAL;
		}
		assert(BigEndian32(path_len) == path_actual_len + 1);

		// Verify path string matches the original file path
		char *path_str = malloc(path_actual_len);
		if (!path_str)
		{
			goto FINAL;
		}
		if (!Ham_FileRead(ham_file, path_str, path_actual_len))
		{
			free(path_str);
			goto FINAL;
		}
		assert(memcmp(path_str, file->Path, path_actual_len) == 0);
		free(path_str);

		// Verify original file size matches
		uint32_t original_size;
		if (!Ham_FileRead(ham_file, &original_size, sizeof(uint32_t)))
		{
			goto FINAL;
		}
		assert(BigEndian32(original_size) == file->Size);

		// Verify compressed size does not exceed zlib upper bound
		uint32_t compressed_size;
		if (!Ham_FileRead(ham_file, &compressed_size, sizeof(uint32_t)))
		{
			goto FINAL;
		}
		assert(BigEndian32(compressed_size) <= compressBound(file->Size));

		// Verify offset is past the header
		uint32_t offset;
		if (!Ham_FileRead(ham_file, &offset, sizeof(uint32_t)))
		{
			goto FINAL;
		}
		assert(BigEndian32(offset) >= HAMCORE_HEADER_SIZE);
	}

	// Verify file offsets are contiguous
	HAMCORE_FILE *files_list = hamcore->Files.List;
	for (int i = 0; i < params->num_files - 1; i++)
	{
		size_t next_offset = files_list[i].Offset + files_list[i].Size;
		assert(next_offset == files_list[i + 1].Offset);
	}

	// Verify last file's data extends exactly to end of archive
	HAMCORE_FILE *last_file = &hamcore->Files.List[params->num_files - 1];
	assert(last_file->Offset + last_file->Size == Ham_FileSize(HAMCORE_NAME));

	HamcoreClose(hamcore);
	return true;

FINAL:
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
