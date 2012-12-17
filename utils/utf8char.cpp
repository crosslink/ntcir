/*
 * utf8char.cpp
 *
 *  Created on: Jan 7, 2010
 *      Author: monfee
 */


#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "string_utils.h"

#include <string>

using namespace std;

int main(int argc, char **argv)
{
	string str;
	char bytes[5];

	char *input = argv[1];

	while (*input != '\0') {
		if (strncmp(input, "0x", 2) == 0) {
			input += 2;
			//cp = strtol(input, NULL, 16);
			//cp = string_to_number<long>(input, 16);
		}
		else if (*input == '\\') {
			++input;
			char c = strtol(input, NULL, 8);
			str.push_back(c);
			while (isdigit(*input))
				++input;
		}
		else {
			str.push_back(' ');
			++input;
		}

	}

	//cpoint_to_utf8bytes(bytes, cp);
	//cout << "Code point: " << cp << endl;
	cout << "Input: \"" <<  str << "\"" << endl;

	return 1;
}
