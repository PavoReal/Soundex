#include <stdio.h>

// #define STB_DS_IMPLEMENTATION
// #include "stb_ds.h"

#include "soundex.h"

int 
main(int argc, char **argv)
{
	for (s32 i = 1; i < argc; ++i)
	{
		char *string = argv[i];
		char buffer[32]    = {0};

		s32 b = Soundex(string, buffer, sizeof(buffer));	

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