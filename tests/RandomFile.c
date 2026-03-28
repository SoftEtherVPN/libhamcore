#include "RandomFile.h"
#include "FileSystem.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint32_t Xorshift32(uint32_t *state)
{
	uint32_t x = *state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	*state = x;
	return x;
}

static uint8_t *MakeRandomBytes(const size_t size)
{
	if (size == 0)
	{
		return NULL;
	}

	uint8_t *buffer = malloc(size);
	if (!buffer)
	{
		return NULL;
	}

	uint32_t state = 12345;

	size_t i = 0;
	for (; i + sizeof(uint32_t) <= size; i += sizeof(uint32_t))
	{
		uint32_t random_byte = Xorshift32(&state);
		memcpy(buffer + i, &random_byte, sizeof(uint32_t));
	}
	for (; i < size; i++)
	{
		buffer[i] = (uint8_t)(Xorshift32(&state) % 256);
	}

	return buffer;
}

RANDOM_FILE *CreateRandomFile(const char *path, const size_t size)
{
	if (!path || size == 0)
	{
		return NULL;
	}

	RANDOM_FILE *random_file = malloc(sizeof(RANDOM_FILE));
	if (!random_file)
	{
		return NULL;
	}
	memset(random_file, 0, sizeof(RANDOM_FILE));

	random_file->Size = size;

	random_file->Path = malloc(strlen(path) + 1);
	if (!random_file->Path)
	{
		goto FINAL;
	}
	strcpy(random_file->Path, path);

	uint8_t *random_bytes = MakeRandomBytes(size);
	if (!random_bytes)
	{
		goto FINAL;
	}

	FILE *file = Ham_FileOpen(path, true);
	if (!file)
	{
		free(random_bytes);
		goto FINAL;
	}

	if (! Ham_FileWrite(file, random_bytes, random_file->Size))
	{
		free(random_bytes);
		Ham_FileClose(file);
		goto FINAL;
	}

	free(random_bytes);
	Ham_FileClose(file);

	return random_file;

FINAL:
	DeleteRandomFile(random_file);
	return NULL;
}

void DeleteRandomFile(RANDOM_FILE *file)
{
	if (!file)
	{
		return;
	}

	if (file->Path)
	{
		remove(file->Path);
		free(file->Path);
	}

	free(file);
}
