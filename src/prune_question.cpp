/*
 * prune_question.cpp
 *
 *  Created on: Mar 24, 2010
 *      Author: monfee
 */

#include <string>
#include <iostream>
#include <iomanip>

#include <nlp/nlp_qa_template.tcc>
#include <utils/icstring.h>

#include <string.h>

#include "sys_file.h"

using namespace std;

const char *lang = NULL;

void remove_questionstopwords(const char *question)
{
	icstring a_question(question);
	//icstring replace = " ";

	int len = 0;
	const char **question_phrase = 0;

	if (strcasecmp(lang, "en") == 0) {
		len = stpl::nlp::question_phrase_english_length;
		question_phrase = stpl::nlp::question_phrase_english;
	}
	else if (strcasecmp(lang, "cs") == 0){
		len = stpl::nlp::question_phrase_chinese_length;
		question_phrase = stpl::nlp::question_phrase_chinese;
	}

	for (int i = 0; i < len; ++i) {
		icstring qpc(question_phrase[i]);
		//if ( != string::npos)
		icstring::size_type pos = a_question.find(qpc);
		if (pos != string::npos) {
			//find_and_replace(a_question, qpc, replace);
			a_question.replace(pos, qpc.length(), "");

			if (lang == "EN" || lang == "en")
				break;
		}
	}
	cout << a_question << endl;
}

void prune_question(const char *file)
{
	char *buf = sys_file::read_entire_file(file);
	remove_questionstopwords(buf);
	delete [] buf;
}

void parse_param(int argc, char **argv)
{
	long param = 0;
	char *command = NULL;

	for (param = 1; param < argc; ++param) {
		if (*argv[param] == '-') {
			command = argv[param] + 1;
			if (strcmp(command, "lang") == 0) {
				lang = argv[++param];
			}
		} else
			break;
	}

	if (lang == 0) {
		cerr << "Usage: " << argv[0] << " -lang [en|cs] file" << endl;
		exit(-1);
	}


	for (; param < argc; param++) {
		prune_question(argv[param]);
	}

}

int main(int argc, char **argv)
{
	parse_param(argc, argv);
	return 0;
}
