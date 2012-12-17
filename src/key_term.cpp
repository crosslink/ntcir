/*
 * key_term.cpp
 *
 *  Created on: Feb 18, 2010
 *      Author: monfee
 */

#include "key_term.h"

#include <stpl/stpl_stream.h>
#include <stpl/xml/stpl_xml.h>
#include <nlp/nlp_tag.h>
#include <nlp/nlp_tag_proxy.h>

#include "string_utils.h"

using namespace stpl;

namespace NTCIR {

//	key_term::key_term()
//	{
//
//	}
//
//	key_term::~key_term()
//	{
//
//	}

	void key_term::load_key_terms(std::string& filename, key_term_array_type& key_terms)
	{
		FileStream<string, char *> fs(filename);
		XML::XParser<string, char *> parser(fs.begin(), fs.end());
		XML::XParser<string, char *>::document_type& doc = parser.parse();

		typedef XML::XParser<string, char *>::document_type::tree_type node_container;
		typedef XML::XParser<string, char *>::document_type::element_type element;

		node_container terms;
		doc.root()->find("KEYTERM", terms);
		node_container::iterator it = terms.begin();
		for (; it != terms.end(); ++it) {
			element *keyterm_elem = ((element *)(*it));
			key_terms.push_back(key_term());
			string text = keyterm_elem->find_child("TEXT")->all_text();
			trim_right(text, " ");
			trim_left(text, " ");

			float score = atof(keyterm_elem->attribute("SCORE").second.c_str());
			string tag = keyterm_elem->attribute("POS_TAG").second;

			key_terms.back().term = text;
			key_terms.back().score = score;
			key_terms.back().id = key_terms.size() - 1;
			key_terms.back().tag_id = stpl::nlp::pos_tag_table[tag];

			node_container trans;
			keyterm_elem->find("TRAN", trans);
			node_container::iterator iner_it = trans.begin();
			for (; iner_it != trans.end(); ++iner_it) {
				element *tran_elem = ((element *)(*iner_it));
				text = tran_elem->all_text();
				trim_right(text, " ");
				trim_left(text, " ");
				string translation_method = tran_elem->attribute("TM").second;
				if (translation_method == "MT")
					key_terms.back().mt_tran = text;
				else if (translation_method == "GW")
					key_terms.back().trans.push_back(text);
			}
		}
	}
}
