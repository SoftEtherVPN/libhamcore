#include "FileSystem.h"

#include <string.h>

#include <sys/stat.h>

FILE *FileOpen(const char *path, const bool write)
{
	if (!path)
	{
		return NULL;
	}

	return fopen(path, write ? "wb" : "rb");
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

bool FileWrite(FILE *file, const void *src, const size_t size)
{
	if (!file || !src || size == 0)
	{
		return false;
	}

	return fwrite(src, 1, size, file) == size;
}

bool FileSeek(FILE *file, const size_t offset)
{
	if (!file)
	{
		return false;
	}

	return fseek(file, offset, SEEK_SET) == 0;
}

size_t FileSize(const char *path)
{
	if (!path)
	{
		return 0;
	}

	struct stat st;
	if (stat(path, &st) == -1)
	{
		return 0;
	}

	return st.st_size;
}

const char *PathRelativeToBase(const char *full, const char *base)
{
	if (!full || !base)
	{
		return NULL;
	}

	if (strstr(full, base) != &full[0])
	{
		return NULL;
	}

	full += strlen(base);
	if (full[0] == '/')
	{
		++full;
	}

	return full;
}
