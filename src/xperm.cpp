#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

inline void
PrintHelp()
{
	const char *usage = "xperm: You must provide at least one name or word to find permutations of";
	puts(usage);
}

struct String
{
	char *str;
	u64 length;
};

struct WordXPair
{
	String word;
	String soundx;	
};

struct WordXList
{
	WordXPair *words;
	u64 wordsCount;
};

void
FreeWordXList(WordXList *list)
{
	if (list->wordsCount)
	{
		free(list->words);
	}
}

WordXList
GenerateWordXList(const char *wordlistPath)
{
	WordXList result = {};

	int fd = open(wordlistPath, O_RDONLY);

	if (fd == -1)
	{
		puts("Could not open wordlist...");
		return result;
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

	close(fd);

	return result;	
}

int
main(int argc, const char **argv)
{
	if (argc <= 1)
	{
		PrintHelp();

		return 0;
	}

	const char **toFind = (const char**) calloc(argc - 1, 1);

	const char *wordlist = 0;

	u32 index = 0;
	for (int i = 1; i < argc; ++i)
	{
		const char *arg = argv[i];

		if (*arg == '-')
		{
			switch (*(++arg))
			{
				case 'w':
				{
					++i;

					if (i >= argc)
					{
						PrintHelp();
					}
					else
					{
						wordlist = argv[i];
					}
				} break;
			}
		}
		else
		{
			toFind[index++] = arg;
		}
	}

	if (!wordlist)
	{
		wordlist = "./words.txt";
	}

	printf("Using wordlist: %s\n", wordlist);

	WordXList wordXList = GenerateWordXList(wordlist);


	u32 in = 0;
	const char *word = toFind[in];

	while (word)
	{
		puts(word);

		word = toFind[++in];	
	}

	FreeWordXList(&wordXList);

	return 0;
}
