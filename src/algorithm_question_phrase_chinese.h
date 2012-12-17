/*
 * algorithm_question_phrase_chinese.h
 *
 *  Created on: Feb 25, 2010
 *      Author: monfee
 */

#ifndef ALGORITHM_QUESTION_PHRASE_CHINESE_H_
#define ALGORITHM_QUESTION_PHRASE_CHINESE_H_

#include "algorithm.h"

namespace NTCIR {

	class algorithm_question_phrase_chinese: public NTCIR::algorithm
	{
	public:
		algorithm_question_phrase_chinese();
		virtual ~algorithm_question_phrase_chinese();

		virtual void start(std::string& question, key_term_array_type& key_terms);
	};

}

#endif /* ALGORITHM_QUESTION_PHRASE_CHINESE_H_ */
