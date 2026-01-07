#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

inline int
IsWhitespace(char c)
{
	return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r');
}

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
	char soundex[5];  // 4 chars + null terminator
};

struct WordXList
{
	WordXPair *words;
	u64 wordsCount;
};

void
FreeWordXList(WordXList *list)
{
	if (list->words)
	{
		for (u64 i = 0; i < list->wordsCount; i++)
		{
			free(list->words[i].word.str);
		}
		free(list->words);
		list->words = NULL;
	}
	list->wordsCount = 0;
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

	u64 fileSize = (u64) st.st_size;
	u64 r = 0;

	u8 *buffer = (u8*) malloc(fileSize + 1);
	if (!buffer)
	{
		close(fd);
		return result;
	}

	while (r < fileSize)
	{
		u64 dr = (u64) read(fd, buffer + r, fileSize - r);
		if (dr == 0) break;
		r += dr;
	}

	close(fd);
	buffer[fileSize] = '\0';

	// First pass: count words
	u64 wordCount = 0;
	int inWord = 0;
	for (u64 i = 0; i < fileSize; i++)
	{
		if (IsWhitespace((char) buffer[i]))
		{
			inWord = 0;
		}
		else if (!inWord)
		{
			inWord = 1;
			wordCount++;
		}
	}

	if (wordCount == 0)
	{
		free(buffer);
		return result;
	}

	// Allocate pairs array
	result.words = (WordXPair*) malloc(sizeof(WordXPair) * wordCount);
	if (!result.words)
	{
		free(buffer);
		return result;
	}
	result.wordsCount = wordCount;

	// Second pass: extract words and compute Soundex codes
	u64 idx = 0;
	u64 wordStart = 0;
	inWord = 0;

	for (u64 i = 0; i <= fileSize; i++)
	{
		char c = (i < fileSize) ? (char) buffer[i] : ' ';

		if (IsWhitespace(c))
		{
			if (inWord)
			{
				// End of word
				u64 len = i - wordStart;

				result.words[idx].word.str = (char*) malloc(len + 1);
				memcpy(result.words[idx].word.str, buffer + wordStart, len);
				result.words[idx].word.str[len] = '\0';
				result.words[idx].word.length = len;

				// Compute Soundex code
				Soundex(result.words[idx].word.str, result.words[idx].soundex);
				result.words[idx].soundex[4] = '\0';

				idx++;
				inWord = 0;
			}
		}
		else if (!inWord)
		{
			// Start of new word
			wordStart = i;
			inWord = 1;
		}
	}

	free(buffer);
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

	const char **toFind = (const char**) calloc(argc - 1, sizeof(const char*));

	const char *wordlist = 0;

	u32 toFindCount = 0;
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
			toFind[toFindCount++] = arg;
		}
	}

	if (!wordlist)
	{
		wordlist = "./words.txt";
	}

	printf("Using wordlist: %s\n", wordlist);

	WordXList wordXList = GenerateWordXList(wordlist);

	if (wordXList.wordsCount == 0)
	{
		puts("No words loaded from wordlist.");
		free(toFind);
		return 1;
	}

	printf("Loaded %lu words from wordlist.\n\n", (unsigned long) wordXList.wordsCount);

	// For each word to find, compute its Soundex and find matches
	for (u32 i = 0; i < toFindCount; i++)
	{
		const char *word = toFind[i];
		char targetSoundex[5];

		Soundex(word, targetSoundex);
		targetSoundex[4] = '\0';

		printf("Words matching '%s' (Soundex: %s):\n", word, targetSoundex);

		u32 matchCount = 0;
		for (u64 j = 0; j < wordXList.wordsCount; j++)
		{
			if (memcmp(targetSoundex, wordXList.words[j].soundex, 4) == 0)
			{
				printf("  %s\n", wordXList.words[j].word.str);
				matchCount++;
			}
		}

		if (matchCount == 0)
		{
			puts("  (no matches found)");
		}
		else
		{
			printf("  [%u matches]\n", matchCount);
		}
		puts("");
	}

	free(toFind);
	FreeWordXList(&wordXList);

	return 0;
}
