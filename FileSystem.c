#include "FileSystem.h"

FILE *FileOpen(const char *path)
{
	if (!path)
	{
		return NULL;
	}

	return fopen(path, "rb");
}

bool FileClose(FILE *file)
{
	if (!file)
	{
		return false;
	}

	return fclose(file) == 0;
}

bool FileRead(FILE *file, void *dst, const size_t size)
{
	if (!file || !dst || size == 0)
	{
		return false;
	}

	return fread(dst, 1, size, file) == size;
}

bool FileSeek(FILE *file, const size_t offset)
{
	if (!file)
	{
		return false;
	}

	return fseek(file, offset, SEEK_SET) == 0;
}
