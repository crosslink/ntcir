/*
 * algorithm_question_phrase_chinese.cpp
 *
 *  Created on: Feb 25, 2010
 *      Author: monfee
 */

#include "algorithm_question_phrase_chinese.h"


#include <string>

using namespace std;

namespace NTCIR {

	algorithm_question_phrase_chinese::algorithm_question_phrase_chinese()
	{

	}

	algorithm_question_phrase_chinese::~algorithm_question_phrase_chinese()
	{

	}

	void algorithm_question_phrase_chinese::start(std::string& question, key_term_array_type& key_terms)
	{
		terms_ = remove_questionstopwords(question, src_lang_);
	}
}
