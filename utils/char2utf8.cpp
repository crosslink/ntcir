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
#include <iomanip>

using namespace std;

int main(int argc, char **argv)
{
	string str;
	int	count = 0;

	char *input = argv[1];
	//unsigned short value = 0;

	cout << "Input: " << input << endl;
	cout << "OCT: \"";
	//cout.setf(std::ios::oct, std::ios::basefield);
	while (*input != '\0') {
		cout << '\\' << setfill('0') << setw(2) << oct << int(static_cast<unsigned char>(*input));
		++input;
	}
	cout << "\"" << endl;

	//cpoint_to_utf8bytes(bytes, cp);
	//cout << "Code point: " << cp << endl;
	input = argv[1];
	cout << "HEX: ";
	//cout.setf(std::ios::hex, std::ios::basefield);
	while (*input != '\0') {
		//value = *input;
		//cout << "0x" << value;
		if (count > 0)
			cout << ", ";
		cout << "(char)0x" <<  setw(2) << hex << int(static_cast<unsigned char>(*input));
		++input;
		++count;
	}
	cout << endl;

	return 1;
}
