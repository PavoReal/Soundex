#include <stdio.h>

#define PEACOCK_SOUNDEX_IMPLEMENTATION
#include "soundex.h"

int 
main(int argc, char **argv)
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