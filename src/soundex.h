#ifndef PEACOCK_SOUNDEX_H
#define PEACOCK_SOUNDEX_H

#include <stdint.h>

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

// From https://en.wikipedia.org/wiki/Soundex#Variants:
//
// 1. Retain the first letter of the name and drop all other occurrences of a, e, i, o, u, y, h, w.
//
// 2. Replace consonants with digits as follows (after the first letter):
//     b, f, p, v → 1
//     c, g, j, k, q, s, x, z → 2
//     d, t → 3
//     l → 4
//     m, n → 5
//     r → 6
//
// 3. If two or more letters with the same number are adjacent in the original name (before step 1), only retain the
// first letter; also two letters with the same number separated by 'h' or 'w' are coded as a single number, whereas
// such letters separated by a vowel are coded twice. This rule also applies to the first letter. 
//
// 4. If you have too few letters in your word that you can't assign three numbers, append with zeros until there are
// three numbers. If you have more than 3 letters, just retain the first 3 numbers.
//
inline s32 
Soundex(const char *input, char *out, s32 outMaxLength = -1)
{
	s32 result = 0; 	

	// { a, b, c, ...} --> { -1, 1, 2, ...}
	s32 CHAR_LOOKUP[] = { -1, 1, 2, 3, -1, 1, 2, -1, -1, 2, 2, 4, 5, 5, -1, 1, 2, 6, 2, 3, -1, 1, -1, 2, -1, 2 };

	char *dest       = out;
	const char *src  = input;

	char firstChar = *src++ & 0xDF;
	s32 firstIndex = firstChar - 'A';

	*dest++ = firstChar;
	++result;

	s32 prevValue = CHAR_LOOKUP[firstIndex];
	while ((*src) && (result <= 3))
	{
		char c = *src & 0xDF;
		s32 index = c - 'A';

		s32 value = CHAR_LOOKUP[index];

		if (value == -1)
		{
			++src;
			prevValue = value;
			continue;
		}

		if (value != prevValue)
		{
			*dest++ = value + '0';
			++result;
			prevValue = value;
		}

		++src;
	}

	for (; result <= 3; ++result)
	{
		*dest++ = '0';
	}

	return result;
}

#endif
