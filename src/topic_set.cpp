/*
 * topic_set.cpp
 *
 *  Created on: Dec 27, 2009
 *      Author: monfee
 */

#include "topic_set.h"
#include <string>

#include <stpl/stpl_stream.h>
#include <stpl/xml/stpl_xml.h>

#include "string_utils.h"

using namespace stpl;
using namespace std;

namespace NTCIR {

	topic_set::topic_set(char *topic_file) {
		parse(topic_file);
	}

	topic_set::~topic_set() {

	}

	void topic_set::parse(char *topic_file) {
		FileStream<string, char *> fs(topic_file);
		XML::XParser<string, char *> parser(fs.begin(), fs.end());
		XML::XParser<string, char *>::document_type& doc = parser.parse();
//		XML::XParser<string, char *>::document_type::entity_iterator begin = doc.iter_begin();
//		for (; begin != doc.iter_end(); ++begin) {
//
//		}
		typedef XML::XParser<string, char *>::document_type::tree_type node_container;
		typedef XML::XParser<string, char *>::document_type::element_type element;
		node_container topics;
		doc.root()->find("TOPIC", topics);
		node_container::iterator it = topics.begin();
		for (; it != topics.end(); ++it) {
			element *topic_elem = ((element *)(*it));

			string topic_id = topic_elem->attribute("ID").second;
			assert(topic_id.length() > 0);
			string topic_title = topic_elem->attribute("TITLE").second;
			topic_array_.push_back(NTCIR::topic(topic_id, topic_title));
			NTCIR::topic& a_topic = topic_array_.back();

			node_container questions;
			topic_elem->find("QUESTION", questions);
			node_container::iterator iner_it = questions.begin();
			for (; iner_it != questions.end(); ++iner_it) {
				element *question_elem = ((element *)(*iner_it));
				string lang = question_elem->attribute("LANG").second;
				string text = question_elem->all_text();
				if (lang == "EN")
					a_topic.set_question_en(text);
				else if (lang == "CS" || lang == "CT")
					a_topic.set_question_zh(text);
			}

			if (topic_elem->find_child("QUESTION_ANALYSIS") != NULL) {
				node_container terms;
				topic_elem->find("KEYTERM", terms);
				node_container::iterator iner_it = terms.begin();
				for (; iner_it != terms.end(); ++iner_it) {
					element *keyterm_elem = ((element *)(*iner_it));
					// two decimal precision
					float score = atof(keyterm_elem->attribute("SCORE").second.c_str());
					// score = (float)((int)(score * 100)) / 100;
					string text = keyterm_elem->all_text();
					trim_right(text, " ");
					trim_left(text, " ");
					a_topic.add_key_term(text, score);
				}
			}
		}
	}

}
