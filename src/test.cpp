#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define PEACOCK_SOUNDEX_IMPLEMENTATION
#include "soundex.h"

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

inline bool
IsWhitespace(char c)
{
	bool result = (c == ' ') || (c == '\r') || (c == '\n') || (c == '\t');

	return result;
}

int 
test_args(int argc, char **argv)
{
	for (int i = 1; i < argc; ++i)
	{
		char *string = argv[i];
		char buffer[32]    = {0};

		int b = Soundex(string, buffer);	

		if (b <= 0)
		{
			printf("Error code %d: Could not calculate soundex code for \"%s\"...\n", b, string);
		}
		else
		{
			printf("%s --> (%d) %s\n", string, b, buffer);
		}
	}

	return 0;
}

int
test_file(const char *path)
{
	int fd = open(path, O_RDONLY);

	if (fd == -1)
	{
		return -1;
	}

	struct stat st;
	fstat(fd, &st);

	u64 fileSize = st.st_size;
	u64 r = 0;

	u8 *buffer = (u8*) malloc(fileSize);

	while (r < fileSize)
	{
		u64 dr = (u64) read(fd, buffer + r, fileSize - r);
		r += dr;
	}

	char *index     = (char*) buffer;
	char *stopIndex = index + fileSize;

	while (index < stopIndex)
	{
		while (IsWhitespace(*index))
		{
			++index;
		}

		char *start = index;

		while (!IsWhitespace(*index) && (index < stopIndex))
		{
			++index;
		}

		*index++ = '\0';

		char result[5] = {0};
		int e = Soundex(start, result);

		if (e)
		{
			printf("%s --> %s\n", start, result);
		}
		else
		{
			printf("%s --> (error)\n", start);
		}
	}

	free(buffer);
	close(fd);

	return 0;
}

int
main(int argc, char **argv)
{
	if (test_file("words.txt") != 0)
	{
		puts("FILE ERROR");
	}

	return test_args(argc, argv);
}
