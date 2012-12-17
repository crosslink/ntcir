/*
 * chinese_segmentation.cpp
 *
 *  Created on: Mar 2, 2010
 *      Author: monfee
 */

#include "chinese_segmentation.h"
#include "language.h"
#include "uniseg_settings.h"
#include "uniseg.h"
#include "uniseg_types.h"

#include <iostream>

using namespace std;

namespace NTCIR {

	chinese_segmentation::chinese_segmentation()
	{

	}

	chinese_segmentation::~chinese_segmentation()
	{

	}

	std::string chinese_segmentation::segment_bigram(const std::string& text)
	{
		const char *start, *here;
		const char *current = text.c_str();
		long word_count = 0, pre_length_of_token = 0;
		std::string after_text;
		while (*current != '\0') {
			if (language::ischinese(current)) {	// Chinese CodePage

				if (after_text.length() > 0 && !isspace(after_text[after_text.length() - 1]))
					after_text.append(" ");

				word_count = 1;
				start = current;
				current += language::utf8_bytes(current);		// move on to the next character

				while (language::ischinese(current)) {	// don't need to check for '\0' because that isn't a Chinese character

					pre_length_of_token = language::utf8_bytes(current);
					current += pre_length_of_token;
					if (++word_count >= 2)
						break;
				}

//				current_token.start = (char *)start;
//				current_token.string_length = current - start;
				after_text.append(std::string(start, current) + " ");

				// post-processing, for bigram indexing, needs move backfoward
				if (*current != '\0' && language::ischinese(current))
					current -= pre_length_of_token;
			}
			else
				after_text.push_back(*current++);
		}
		return after_text;
	}

	std::string chinese_segmentation::segment_ngmi(const std::string& text)
	{
		const char *start, *here;
		const char *current = text.c_str();
		long word_count = 0, pre_length_of_token = 0;
		std::string after_text;
		while (*current != '\0') {
			if (language::ischinese(current)) {	// Chinese CodePage

				word_count = 1;
				start = current;
				current += language::utf8_bytes(current);		// move on to the next character

				while (language::ischinese(current))
					current += language::utf8_bytes(current);

				uniseg_encoding *enc = uniseg_encoding_factory::instance().get_encoding();

				UNISEG_settings::instance().max = 11;
				static UNISEG_uniseg uniseg;

				const char *output = (const char *)uniseg.do_segmentation((unsigned char *)start, current - start);
				if (strlen(output) < current - start)
					cerr << ("Warning: length of output is less than the terms' length, output: ") << output << endl;
				after_text.append(output);
			}
			else
				after_text.push_back(*current++);
		}
		return after_text;
	}

	std::string chinese_segmentation::segment_single(const std::string& text)
	{
		const char *start, *here;
		const char *current = text.c_str();
		long word_count = 0, pre_length_of_token = 0;
		std::string after_text;
		while (*current != '\0') {
			if (language::ischinese(current)) {	// Chinese CodePage

				word_count = 1;
				start = current;
				current += language::utf8_bytes(current);		// move on to the next character

				after_text.append(std::string(start, current) + " ");
			}
			else
				after_text.push_back(*current++);
		}
		return after_text;
	}
}
