// This is a WIP, and isn't guaranteed to be correct, fast, or even functional...

//
// This is a stb style header, to use this library define PEACOCK_SOUNDEX_IMPLEMENTATION
// in *one* c or c++ file.
//
// Docs:
//
// int Soundex(const char *input, char *out)
//     Calculates the original soundex code (https://en.wikipedia.org/wiki/Soundex#Variants) for the c-string (input),
//     storing the result in (out). (out) *must* have at least a capacity of 4 bytes, one for the first char and three
//     for the digits. Returns the number of bytes written, should always be 4.
//     DOES *NO* ERROR CHECKING
//

#ifndef PEACOCK_SOUNDEX_H
#define PEACOCK_SOUNDEX_H

#ifdef __cplusplus
extern "C" {
#endif

extern int Soundex(const char *input, char *out);

//
// IMPLEMENTATION
//

#ifdef PEACOCK_SOUNDEX_IMPLEMENTATION

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
int 
Soundex(const char *input, char *out)
{
	int result = 0; 	

	// { a, b, c, ...} --> { -1, 1, 2, ...}
	int CHAR_LOOKUP[] = { -1, 1, 2, 3, -1, 1, 2, -1, -1, 2, 2, 4, 5, 5, -1, 1, 2, 6, 2, 3, -1, 1, -1, 2, -1, 2 };

	char *dest       = out;
	const char *src  = input;

	char firstChar = *src++ & 0xDF;
	int firstIndex = firstChar - 'A';

	*dest++ = firstChar;
	++result;

	int prevValue = CHAR_LOOKUP[firstIndex];
	while ((*src) && (result <= 3))
	{
		char c = *src & 0xDF;
		int index = c - 'A';

		int value = CHAR_LOOKUP[index];

		if (value == -1)
		{
			// Per step 3: 'h' and 'w' are transparent for duplicate detection,
			// so we do NOT reset prevValue for them. Vowels (a, e, i, o, u, y)
			// DO reset prevValue, allowing same-coded consonants to be coded twice.
			if (c != 'H' && c != 'W')
			{
				prevValue = value;
			}
		}
		else if (value != prevValue)
		{
			*dest++ = (char) (value + '0');
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

#ifdef __cplusplus
}
#endif

#endif
