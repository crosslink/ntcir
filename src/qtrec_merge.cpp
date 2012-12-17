/*
 * qtrec_merge.cpp
 *
 *  Created on: Mar 8, 2010
 *      Author: monfee
 */

#include "chinese_segmentation.h"
#include "sys_file.h"

#include <string.h>
#include <libgen.h>

#include <iostream>
#include <fstream>

using namespace std;

void merge(const char *out_path, const char *file)
{
	const char *buf = sys_file::read_entire_file(file);
	const char *sig1 = "<TEXT>";
	const char *sig2 = "</TEXT>";
	const char *start, *stop, *curr = buf;
	char bname[1024];

	strcpy(bname, file);
	const char *rbname = basename(bname);
	string output_file = string(rbname) + ".mixed";
	output_file.insert(0, string(out_path) + sys_file::SEPARATOR);

	ofstream os(output_file.c_str(), ios::out | ios::trunc);

	while ((start = strstr(curr, sig1)) != NULL) {
		start += strlen(sig1);
		stop = strstr(start, sig2);

		if (!stop) {
			cerr << "Unmatching </TEXT>" << endl;
			continue;
		}

		os << string(curr, start);
		string text(start, stop);

		// three times segmentation
		os << NTCIR::chinese_segmentation::segment_single(text) << endl;
		os << NTCIR::chinese_segmentation::segment_bigram(text) << endl;
		os << NTCIR::chinese_segmentation::segment_ngmi(text);
//		os << sig2;

		curr = stop;
	}

	os << curr;

	delete [] buf;
}

void parse_param(int argc, char **argv)
{
	long param = 0;
	char *command = NULL;
	const char *out_path;

	for (param = 1; param < argc; param++) {
		if (*argv[param] == '-') {
			command = argv[param] + 1;
			if (strcmp(command, "out") == 0) {
				out_path = argv[++param];
				++param;
				break;
			}
		}
	}

	if (!out_path) {
		cerr << "Out path NULL!" << endl;
		exit(-1);
	}

	for (; param < argc; param++)
		merge(out_path, argv[param]);
}

int main(int argc, char **argv)
{
	parse_param(argc, argv);
	return 0;
}

