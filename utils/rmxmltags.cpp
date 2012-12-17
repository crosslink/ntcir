#include <stdio.h>

#include "ant_link_parts.h"
#include "../src/sys_file.h"

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s input", argv[0]);
	}

	char *input = sys_file::read_entire_file(argv[1]);

	string_clean_tag(input, 0);

	puts(input);

	delete [] input;

	return 0;
}
