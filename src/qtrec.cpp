/*
 * qtrec.cpp
 *
 *  Created on: Dec 28, 2009
 *      Author: monfee
 */

#include <string>
#include <iostream>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "topic_set.h"
#include "application_settings.h"

using namespace NTCIR;
using namespace std;

enum { CHINESE, ENGLISH };

void usage(char *program)
{
	printf("Usage:%s [option...] filename <... filename>\n", program);
	//printf("     : -? for help\n");
	puts("");
	puts("-topic		The ACLIA topic file, this option has to come first");
	puts("-list			List the ACLIA topic file");
	puts("-n			The number of results");
	exit(0);
}

topic_set *parse_topic(char *topic_file)
{
	return new topic_set(topic_file);
}

void list_topic(topic_set *t_set)
{
	std::vector<topic>& topic_array = t_set->topic_array();
	for (int i = 0; i < topic_array.size(); i++)
		cerr << topic_array[i].get_id() << " " << topic_array[i].get_title() << endl;
}

void print_header(const char *run_id)
{
	string header = application_settings::instance().attributeof("output_header");
	printf(header.c_str(), run_id);
}

void print_footer()
{
	string footer = application_settings::instance().attributeof("output_footer");
	puts(footer.c_str());
}

void ir(topic_set *t_set, const char *run_id, int number_to_list)
{
	print_header(run_id);
	std::vector<topic>& topic_array = t_set->topic_array();
	for (int i = 0; i < topic_array.size(); i++) {
		topic_array[i].set_number_to_list(number_to_list);
//		switch(lang) {
//			case CHINESE:
//				topic_array[i].retrieve_zh(run_id);
//				break;
//			case ENGLISH:
//				topic_array[i].retrieve_en(run_id);
//				break;
//			default:
//				break;
//		}
		topic_array[i].retrieve(run_id);
	}
	print_footer();
}

void parse_param(int argc, char **argv)
{
	long param = 0;
	char *command = NULL;
	topic_set *t_set = NULL;
	const char *run_id = "unknown_run_id";
	int number_to_list = atoi(application_settings::instance().attributeof("output_document_number").c_str());

	if (number_to_list < 0)
		number_to_list = 10;

	for (param = 1; param < argc; param++) {
		if (*argv[param] == '-') {
			command = argv[param] + 1;
			if (strcmp(command, "topic") == 0) {
				t_set = parse_topic(argv[++param]);
			} else if (strcmp(command, "run") == 0) {
				run_id = argv[++param];
			} else if (*command == 'n')
				number_to_list = atoi(argv[++param]);
			if (strcmp(command, "list") == 0) {
				if (!t_set)
					usage(argv[0]);
				list_topic(t_set);
			}
		}
	}

	ir(t_set, run_id, number_to_list);

}

int main(int argc, char **argv)
{
	parse_param(argc, argv);
	return 0;
}
