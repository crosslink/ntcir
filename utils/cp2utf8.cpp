/*
 * cp2utf8.cpp
 *
 *  Created on: Jan 5, 2010
 *      Author: monfee
 */

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "string_utils.h"

using namespace std;

int main(int argc, char **argv)
{
	char bytes[5];

	char *input = argv[1];
	long cp = 0;

	if (strncmp(input, "0x", 2) == 0) {
		input += 2;
		cp = strtol(input, NULL, 16);
		//cp = string_to_number<long>(input, 16);
	}
	else
		cp = atol(input);

	cpoint_to_utf8bytes(bytes, cp);
	cout << "Code point: " << cp << endl;
	cout << "Input: " <<  bytes << endl;

	return 1;
}
