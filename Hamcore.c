#include "Hamcore.h"

#include "FileSystem.h"
#include "Memory.h"

#include <stdlib.h>
#include <string.h>

#include <zlib.h>

HAMCORE *HamcoreOpen(const char *path)
{
	if (!path)
	{
		return NULL;
	}

	HAMCORE *hamcore = malloc(sizeof(HAMCORE));
	memset(hamcore, 0, sizeof(HAMCORE));

	hamcore->File = FileOpen(path);
	if (!hamcore->File)
	{
		free(hamcore);
		return NULL;
	}

	bool ok = false;

	uint8_t header[HAMCORE_HEADER_SIZE];
	if (!FileRead(hamcore->File, header, sizeof(header)))
	{
		goto FINAL;
	}

	if (memcmp(header, HAMCORE_HEADER_DATA, sizeof(header)) != 0)
	{
		goto FINAL;
	}

	uint32_t tmp;
	if (!FileRead(hamcore->File, &tmp, sizeof(tmp)))
	{
		goto FINAL;
	}

	HAMCORE_FILES *files = &hamcore->Files;

	files->Num = BigEndian32(tmp);
	files->List = malloc(sizeof(HAMCORE_FILE) * files->Num);
	memset(files->List, 0, sizeof(HAMCORE_FILE) * files->Num);

	for (size_t i = 0; i < files->Num; ++i)
	{
		if (!FileRead(hamcore->File, &tmp, sizeof(tmp)))
		{
			goto FINAL;
		}

		HAMCORE_FILE *file = &files->List[i];

		tmp = BigEndian32(tmp);
		file->Path = malloc(tmp);
		if (tmp >= 1)
		{
			memset(file->Path, 0, tmp);
			--tmp;
		}

		if (!FileRead(hamcore->File, file->Path, tmp))
		{
			goto FINAL;
		}

		if (!FileRead(hamcore->File, &tmp, sizeof(tmp)))
		{
			goto FINAL;
		}

		file->OriginalSize = BigEndian32(tmp);

		if (!FileRead(hamcore->File, &tmp, sizeof(tmp)))
		{
			goto FINAL;
		}

		file->Size = BigEndian32(tmp);

		if (!FileRead(hamcore->File, &tmp, sizeof(tmp)))
		{
			goto FINAL;
		}

		file->Offset = BigEndian32(tmp);
	}

	ok = true;
FINAL:
	if (!ok)
	{
		HamcoreClose(hamcore);
		hamcore = NULL;
	}

	return hamcore;
}

void HamcoreClose(HAMCORE *hamcore)
{
	if (!hamcore)
	{
		return;
	}

	FileClose(hamcore->File);

	HAMCORE_FILES *files = &hamcore->Files;
	if (!files->List)
	{
		return;
	}

	for (size_t i = 0; i < files->Num; ++i)
	{
		HAMCORE_FILE *file = &files->List[i];
		if (file->Path)
		{
			free(file->Path);
		}
	}

	free(files->List);
	free(hamcore);
}

const HAMCORE_FILE *HamcoreFind(const HAMCORE *hamcore, const char *path)
{
	if (!hamcore || !path)
	{
		return NULL;
	}

	const HAMCORE_FILES *files = &hamcore->Files;

	for (size_t i = 0; i < files->Num; ++i)
	{
		const HAMCORE_FILE *file = &files->List[i];
		if (strcmp(file->Path, path) == 0)
		{
			return file;
		}
	}

	return NULL;
}

bool HamcoreRead(HAMCORE *hamcore, void *dst, const HAMCORE_FILE *hamcore_file)
{
	if (!hamcore || !dst || !hamcore_file)
	{
		return false;
	}

	if (!FileSeek(hamcore->File, hamcore_file->Offset))
	{
		return false;
	}

	bool ok = false;

	void *buf = malloc(hamcore_file->Size);
	if (!FileRead(hamcore->File, buf, hamcore_file->Size))
	{
		goto FINAL;
	}

	uLong dst_size = (uLong)hamcore_file->OriginalSize;
	if (uncompress(dst, &dst_size, buf, (uLong)hamcore_file->Size) != Z_OK)
	{
		goto FINAL;
	}

	if (dst_size != hamcore_file->OriginalSize)
	{
		goto FINAL;
	}

	ok = true;
FINAL:
	free(buf);
	return ok;
}
