/*
 * qqa.cpp
 *
 *  Created on: Feb 15, 2010
 *      Author: monfee
 *
 *      The POS tags used in query generation are:
 *      CD, FW, NN*, VB*
 */

#include <string>
#include <iostream>
#include <iomanip>
#include <map>

#include <string.h>
#include <libgen.h>

#include <stpl/stpl_stream.h>
#include <stpl/xml/stpl_xml.h>
#include <nlp/nlp_tag_processing.h>
#include <nlp/nlp_tag_proxy.h>
#include <utils/icstring.h>

#include "sys_file.h"
#include "string_utils.h"

using namespace std;
using namespace stpl;

const char *description = "This a description";
const char *run_id = "unknown";
bool prune_question = false;

void process_question(const char *question_file, map<std::string, std::string>& topic_map)
{
	const char *file_buf = sys_file::read_entire_file(question_file);
	stpl::nlp::tagged_word_array_type wt_array = stpl::nlp::convert_to_word_tag_array(file_buf);
	//string restored = stpl::nlp::restore_tagged_version(file_buf);
	delete [] file_buf;

	char *question_file_copy = strdup(question_file);
	char *topic_id = basename(question_file_copy);
	char *period = strchr(topic_id, '.');
	if (period != NULL)
		*period = '\0';

//	for (int i = 0; i < wt_array.size(); i++)
//		cerr << wt_array[i].first << " ";
//	cerr << endl;
	map<std::string, std::string>::iterator it = topic_map.find(topic_id);
	string restored = it->second;
	// for debug
	cerr << "Untagged version: ";
	cerr << restored << endl;

	//<ANSWERTYPE SCORE="1.0">DEFINITION</ANSWERTYPE>
	printf("\t<TOPIC ID=\"%s\">\n", topic_id);
	printf("\t\t<QUESTION LANG=\"EN\">%s</QUESTION>\n", restored.c_str());
    puts("\t\t<QUESTION_ANALYSIS>\n \
      \
      \t\t<KEYTERMS LANGUAGE=\"EN\">\n");

    bool has_quote, has_bracket;
    bool is_key_term;
    string pre_term;
    string curr_term;
    string key_term("");

    for (int i = 0; i < wt_array.size();) {
    	curr_term = wt_array[i].first;
    	int tag_id = stpl::nlp::pos_tag_table[wt_array[i].second];
//    	if (tag_id == stpl::nlp::CD
//    			|| tag_id == stpl::nlp::FW
//				|| tag_id ==  stpl::nlp::VB
//				|| tag_id == stpl::nlp::VBD
//				|| tag_id == stpl::nlp::VBG
//				|| tag_id == stpl::nlp::VBN
//				|| tag_id == stpl::nlp::NN
//				|| tag_id == stpl::nlp::NNS
//				|| tag_id ==
//				|| tag_id ==
//				|| tag_id ==
//				|| tag_id ==
//						)
    	float score = 0.0;
        string tag = wt_array[i].second;

        // assign score;
    	switch (tag_id) {
    		case stpl::nlp::RB:
    		case stpl::nlp::RBR:
    		case stpl::nlp::RBS:
				score = 0.4;
				break;
    		case stpl::nlp::CD:
    		case stpl::nlp::FW:
    		case stpl::nlp::VB:
    		case stpl::nlp::VBD:
    		case stpl::nlp::VBP:
    		case stpl::nlp::JJ:
    		case stpl::nlp::JJR:
    		case stpl::nlp::JJS:
    			score = 0.6;
    			break;
    		case stpl::nlp::VBG:
    		case stpl::nlp::VBN:
    		case stpl::nlp::NN:
    		case stpl::nlp::NNS:
    			score = 0.8;
				break;
    		case stpl::nlp::NNP:
    		case stpl::nlp::NNPS:
    			score = 1.0;
    		case stpl::nlp::LEFT_QUOTATION:
    		//case stpl::nlp:::RIGHT_QUOTATION:
    			score = 1.0;
    			break;
    		case stpl::nlp::LEFT_BRACKET:
    			score = 0.99;
    			break;
    		default:
    			score = 0.0;
    			break;
    	}

    	switch (tag_id) {
    		case stpl::nlp::RB:
    		case stpl::nlp::RBR:
    		case stpl::nlp::RBS:
    		case stpl::nlp::CD:
    		case stpl::nlp::FW:
    		case stpl::nlp::VB:
    		case stpl::nlp::VBD:
    		case stpl::nlp::VBP:
    		case stpl::nlp::JJ:
    		case stpl::nlp::JJR:
    		case stpl::nlp::JJS:
    			if (icstring(pre_term.c_str()) == "the") {
    				tag = stpl::nlp::pos_tags[stpl::nlp::NN];
    				key_term = "the ";
    			}
    			if (icstring(wt_array[i].first.c_str()) == icstring("was")
    					|| icstring(wt_array[i].first.c_str()) == icstring("were")
    							|| icstring(wt_array[i].first.c_str()) == icstring("please")) {
    				++i;
    				break;
    			}
				key_term.append(wt_array[i++].first);
    			break;
    		case stpl::nlp::VBG:
    		case stpl::nlp::VBN:
    		case stpl::nlp::NN:
    		case stpl::nlp::NNS:
				key_term.append(wt_array[i++].first);
//				if (stpl::nlp::pos_tag_table[wt_array[i].second] == stpl::nlp::POS) {
//					key_term.append("'s");
//					++i;
//				}
				break;
    		case stpl::nlp::NNP:
    		case stpl::nlp::NNPS:
    			while (stpl::nlp::pos_tag_table[wt_array[i].second] == stpl::nlp::NNP
    					|| stpl::nlp::pos_tag_table[wt_array[i].second] == stpl::nlp::NNPS) {
    				key_term.append(wt_array[i].first + " ");
    				++i;
    				if (i >= wt_array.size())
    					break;
    			}
    			break;
    		case stpl::nlp::LEFT_QUOTATION:
    		//case stpl::nlp:::RIGHT_QUOTATION:
    			tag = stpl::nlp::pos_tags[stpl::nlp::NNP];
    			while (stpl::nlp::pos_tag_table[wt_array[++i].second] != stpl::nlp::RIGHT_QUOTATION)
    				key_term.append(wt_array[i].first + " ");
    			i++;
    			break;
    		case stpl::nlp::LEFT_BRACKET:
    			tag = stpl::nlp::pos_tags[stpl::nlp::NNP];
    			while (stpl::nlp::pos_tag_table[wt_array[++i].second] != stpl::nlp::RIGHT_BRACKET)
    				key_term.append(wt_array[i].first + " ");
    			i++;
    			break;
    		default:
    			i++;
    			break;
    	}
    	pre_term = curr_term;
    	trim_left(pre_term, " ");
    	trim_right(pre_term, " ");
    	if (key_term.length() > 0) {
			cout <<  "\t\t\t\t<KEYTERM SCORE=\"" << fixed << setprecision (2) <<score << "\" POS_TAG=\""<< tag << "\">";
			cout << key_term;
			cout << "</KEYTERM>\n ";
    	}
    	key_term.clear();
    }

	puts("\t\t\t</KEYTERMS>\n \
    \t\t</QUESTION_ANALYSIS>\n \
	\t</TOPIC>\n");
	free(question_file_copy);
}

void load_topics(const char *topic_file, map<std::string, std::string>& topic_map)
{
	FileStream<string, char *> fs(topic_file);
	XML::XParser<string, char *> parser(fs.begin(), fs.end());
	XML::XParser<string, char *>::document_type& doc = parser.parse();

	typedef XML::XParser<string, char *>::document_type::tree_type node_container;
	typedef XML::XParser<string, char *>::document_type::element_type element;
	node_container topics;
	doc.root()->find("TOPIC", topics);
	node_container::iterator it = topics.begin();
	for (; it != topics.end(); ++it) {
		element *topic_elem = ((element *)(*it));

		string topic_id = topic_elem->attribute("ID").second;
		assert(topic_id.length() > 0);

		node_container questions;
		topic_elem->find("QUESTION", questions);
		node_container::iterator iner_it = questions.begin();
		for (; iner_it != questions.end(); ++iner_it) {
			element *question_elem = ((element *)(*iner_it));
			string lang = question_elem->attribute("LANG").second;
			string text = question_elem->all_text();
			if (lang == "EN")
				topic_map.insert(make_pair(topic_id, text));
//			else if (lang == "CS" || lang == "CT")
//				a_topic.set_question_zh(text);
		}
	}
}

void parse_param(int argc, char **argv)
{
	long param = 0;
	char *command = NULL;
	map<string, string> topic_map;

	for (param = 1; param < argc; ++param) {
		if (*argv[param] == '-') {
			command = argv[param] + 1;
			if (strcmp(command, "run") == 0) {
				run_id = argv[++param];
			} else if (strcmp(command, "d") == 0) {
				description = argv[++param];
			} else if (strcmp(command, "prune") == 0) {
				prune_question = true;
			} else if (strcmp(command, "topic") == 0)
				load_topics(argv[++param], topic_map);
		} else
			break;
	}

	if (topic_map.size() == 0) {
		cerr << "Usage: " << argv[0] << " -topic topic_file file..." << endl;
		exit(-1);
	}

	printf("<TOPIC_SET>\n \
		  <METADATA>\n \
			<RUNID>%s</RUNID>\n \
			<DESCRIPTION>%s</DESCRIPTION>\n \
		  </METADATA>\n \
		", run_id, description);

	for (; param < argc; param++) {
		process_question(argv[param], topic_map);
	}

	puts("</TOPIC_SET>");
}

int main(int argc, char **argv)
{
	parse_param(argc, argv);
	return 0;
}
