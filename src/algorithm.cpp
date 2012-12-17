/*
 * algorithm.cpp
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee
 */

#include "algorithm.h"
#include "chinese_segmentation.h"
#include "language.h"
#include "application_settings.h"
#include "string_utils.h"
#include "sys_file.h"
#include "query.h"
#include "google_translator.h"

#include "string_utils.h"

#include <nlp/nlp_qa_template.tcc>
#include <utils/icstring.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <set>

using namespace NTCIR;
using namespace std;

const char *algorithm::CHINESE_QUERY_TERMS_FILE = "chinese_query_terms.txt";
const char *algorithm::CHINESE_QUERY_ANALYSIS_FILE = "chinese_query_terms.xml";

algorithm::algorithm()
{
	query_segmentation_method_ = atoi(application_settings::instance().attributeof("query_segmentation").c_str());
	//need_google_translate_ = application_settings::instance().attributeof("google_translate") == "yes";
	use_pruned_question_ = application_settings::instance().attributeof("use_pruned_question") == "yes";
	use_entire_question_ = application_settings::instance().attributeof("use_entire_question") == "yes";
	prune_after_translation_ = application_settings::instance().attributeof("prune_after_translation") == "yes";
	prune_question_ = application_settings::instance().attributeof("prune_question") == "yes";
	use_pruned_translation_ = false; //application_settings::instance().attributeof("use_pruned_translation") == "yes";

	langpair_ = application_settings::instance().attributeof("LANGPAIR");
	if (langpair_.find("tw") != string::npos || langpair_.find("TW") != string::npos) {
		application_settings::instance().is_ct(true);
		locale_ = "tw";
	}
	else
		locale_ = "cn";
}

algorithm::~algorithm()
{
}

const std::string& algorithm::get_terms()
{
#ifdef DEBUG
	cerr << "machine translation: " << machine_translation_ << endl;
	cerr << "machine translation on pruned question: " << machine_translation_pruned_ << endl;
	cerr << "wikipedia translation: " << wiki_translation_ << endl;
	cerr << "cluetext translation: " << cluetext_translation_ << endl;
#endif
	cerr <<
	terms_.append(" ");

	if (prune_after_translation_) {
		if (!use_pruned_translation_)
			terms_.append(remove_questionstopwords(machine_translation_, dest_lang_));
		else
			terms_.append(remove_questionstopwords(machine_translation_pruned_, dest_lang_));
	}
	else {
		if (!use_pruned_translation_)
			terms_.append(machine_translation_);
		else
			terms_.append(machine_translation_pruned_);
	}

	terms_.append(" ");
	terms_.append(wiki_translation_);

	terms_.append(" ");
	terms_.append(cluetext_translation_);

	terms_.append(" ");
	terms_.append(expansion_terms_);

	if (query_segmentation_method_ > 0) {
		string query = terms_;
		terms_.clear();

		if ((query_segmentation_method_ & SEGMENTATION_BIGRAM) == SEGMENTATION_BIGRAM)
			terms_.append(chinese_segmentation::segment_bigram(query));

		if (terms_[terms_.length() - 1] != ' ')
			terms_.append(" ");

		if ((query_segmentation_method_ & SEGMENTATION_NGMI) == SEGMENTATION_NGMI)
			terms_.append(chinese_segmentation::segment_ngmi(query));

		if (terms_[terms_.length() - 1] != ' ')
			terms_.append(" ");

		if ((query_segmentation_method_ & SEGMENTATION_SINGLE) == SEGMENTATION_SINGLE)
			terms_.append(chinese_segmentation::segment_single(query));

		trim_right(terms_, " ");
		if (terms_.length() == 0) {
			cerr << "Improper segmentation method set, please check your config file. Use the original query instead" << endl;
			terms_ = query;
		}

		create_unique_query_terms(terms_);
	}
	return terms_;
}

void algorithm::create_unique_query_terms(string& terms)
{
	std::vector<string> term_array;
	std::set<string> term_set;
	const char *current = terms.c_str();
	const char *stop = current;
	while (*current != '\0') {
		while (isspace(*current))
				++current;

		stop = current;
		while (*stop != '\0' && !isspace(*stop))
			++stop;

		string tmp(current, stop);

		if (term_set.find(tmp) == term_set.end()) {
			term_set.insert(tmp);
			term_array.push_back(tmp);
		}

		current = stop;
	}

	terms.clear();
	for (int i = 0; i < term_array.size(); ++i) {
		if (i > 0)
			terms.append(" ");
		terms.append(term_array[i]);
	}
}

void algorithm::start(std::string& question, key_term_array_type& key_terms)
{
	string this_question;

	if (prune_question_)
		this_question = remove_questionstopwords(question, src_lang_);
	else
		this_question = question;

	if (src_lang_ != dest_lang_) {
		if (use_pruned_question_ && key_terms.size() > 0) {
			string trans = get_mt(key_terms);
			if (prune_after_translation_)
				machine_translation_pruned_ = remove_questionstopwords(trans, dest_lang_);
			else
				machine_translation_pruned_ = trans;
		}

		if (use_entire_question_)
			get_mt_question(this_question);
	} else
		terms_ = this_question;
}

void algorithm::write_cache(const char *content, std::string filename)
{
	if (!content)
		return;

	fstream file_op(filename.c_str(), ios::out | ios::trunc);

	file_op << content;
	file_op.close();
}

const char *algorithm::get_google_translate_and_save_cache(std::string& term) {
	string file_name = term;
	process_filename(file_name);

	string google_translate_cache_file = create_google_translate_cache_filename(file_name);
	return get_google_translate_and_save_cache(term, google_translate_cache_file);
}

const char *algorithm::get_google_translate_and_save_cache(std::string& term, std::string& filename) {
	const char *content = NULL;
	if (!sys_file::exist(filename.c_str()) || application_settings::instance().override_cache()) {
		string tran = google_translator().translate(term.c_str(), langpair_.c_str());
		write_cache(tran.c_str(), filename);
	}
	return query::retrieve_localfile(filename.c_str());
}

std::string algorithm::create_google_translate_cache_filename(std::string name)
{
	return cache_path_ +  sys_file::SEPARATOR + "google_translate_" + name + ".txt";
}

void algorithm::process_filename(std::string& file_name)
{
	std::replace_if(file_name.begin(), file_name.end(), std::bind2nd(std::equal_to<char>(),'"'), '~') ;
	std::replace_if(file_name.begin(), file_name.end(), std::bind2nd(std::equal_to<char>(),' '), '_') ;
}

void algorithm::get_mt_question(std::string& question)
{
	get_mt_question(question, machine_translation_);
}

void algorithm::get_mt_question(std::string& question, std::string& where_to_save)
{
	if (question.length() > 0)
		where_to_save = get_google_translate_and_save_cache(question);
	language::remove_xml_entities(where_to_save);
}

std::string algorithm::create_text_for_translation(std::vector<key_term>& term_array)
{
	string text;
	for (int i = 0; i < term_array.size(); ++i) {
		text.append(term_array[i].term);
		if (term_array.size() > 0 && i != (term_array.size() - 1))
			text.append(",");
	}
	return text;
}

std::string algorithm::get_mt(key_term_array_type& term_array)
{
	string text = create_text_for_translation(term_array);
	if (text.length() == 0)
		return "";

	string file_name = text;
	std::replace_if(file_name.begin(), file_name.end(), std::bind2nd(std::equal_to<char>(),' '), '_') ;
	std::replace_if(file_name.begin(), file_name.end(), std::bind2nd(std::equal_to<char>(),','), '~') ;
	string google_translate_cache_file = cache_path_ +  sys_file::SEPARATOR + "google_translate_" + file_name + ".txt";
	string terms;

	if (!sys_file::exist(google_translate_cache_file.c_str())) {

		terms = google_translator().translate(text.c_str(), langpair_.c_str());
		//char comma[4] = {(char)0xEF, (char)0xBC, (char)0x8C, (char)0x0A};
		char comma[] = {'\357', '\274', '\214'};

//		while ((pos = terms.find(comma, pos)) != string::npos)
//			terms.replace(pos, )
		//std::replace_if(terms.begin(), terms.end(), std::bind2nd(std::equal_to<string>(), comma), ",");
		find_and_replace(terms, string(comma), string(","));

		write_cache(terms.c_str(), google_translate_cache_file);
	}

	terms = query::retrieve_localfile(google_translate_cache_file.c_str());
	return terms;
}

void algorithm::save_query_terms(key_term_array_type& key_terms)
{
//		FILE *file = fopen(chinese_query_terms_file_.c_str(), "w");
//		FILE *xmlfile = fopen(chinese_query_terms_xmlfile_.c_str(), "w");
//		string text;
//
//	    fprintf(xmlfile, "\t\t<QUESTION_ANALYSIS>\n \
//	      \
//	      <KEYTERMS LANGUAGE=\"%s\">\n", dest_lang_.c_str());
//	    for (int i = 0; i < key_terms.size(); ++i) {
//	    	fprintf(xmlfile, "\t\t\t\t<KEYTERM SCORE=\"%.1f\">\n", key_terms[i].score);
//	    	fprintf(xmlfile, "<TEXT>%s</TEXT>", key_terms[i].term);
//	    	for (int j = 0; j < key_terms[i].trans.size(); ++j) {
//	    		// write xml file
//
//	    		fprintf(xmlfile, "<TRAN TM=\"%s\">%s</TRAN>", "GW", key_terms[i].trans[j].c_str());
//	    		text.append(key_terms[i].trans[j]);
//		    	if (key_terms[i].trans.size() > 0 && j != (key_terms[i].trans.size() - 1))
//		    		text.append(";"); // translation and query expansion using google and Wikipedia
//	    	}
//	    	if (key_terms[i].mt_tran.length() > 0) {
//	    		if (key_terms[i].trans.size() > 0)
//	    			text.append(":"); // for machine translation
//	    		fprintf(xmlfile, "<TRAN TM=\"%s\">%s</TRAN>", "MT", key_terms[i].mt_tran.c_str());
//	    		//fprintf(xmlfile, "\t\t\t\t<KEYTERM SCORE=\"%0.1f\" TM=\"%s\">%s</KEYTERM>\n ", key_terms[i].score, "MT", key_terms[i].mt_tran.c_str());
//	    		text.append(key_terms[i].mt_tran.c_str());
//	    	}
//	    	if (mt_terms_.size() > 1 && i != (mt_terms_.size() - 1))
//	    		text.append(",");
//	    	fprintf(xmlfile, "\t\t\t\t</KEYTERM>\n ");
//	    }
//
//		fputs("\t\t\t</KEYTERMS>\n \
//	    \t\t</QUESTION_ANALYSIS>", xmlfile);
//
//		fputs(text.c_str(), file);
//		fclose(file);
//		fclose(xmlfile);

	string chinese_query_terms_file = cache_path_ +  sys_file::SEPARATOR + CHINESE_QUERY_TERMS_FILE;
	string chinese_query_terms_xmlfile = cache_path_ +  sys_file::SEPARATOR + CHINESE_QUERY_ANALYSIS_FILE;

	ofstream file(chinese_query_terms_file.c_str(), ios::out | ios::trunc);
	ofstream xmlfile(chinese_query_terms_xmlfile.c_str(), ios::out | ios::trunc);
	string text;

	xmlfile << "\t\t<QUESTION_ANALYSIS>" << endl << "\t\t\t<KEYTERMS LANGUAGE=\"" << dest_lang_ << "\">" << endl;
	for (int i = 0; i < key_terms.size(); ++i) {
		xmlfile << "\t\t\t\t<KEYTERM SCORE=\"" << fixed << setprecision(2) << key_terms[i].score << "\">" << endl;
		xmlfile << "<TEXT>" << key_terms[i].term << "</TEXT>" << endl;
		for (int j = 0; j < key_terms[i].trans.size(); ++j) {
			// write xml file

			xmlfile << "\t\t\t\t\t<TRAN TM=\"" << "GW" << "\">" << key_terms[i].trans[j]  << "</TRAN>"<< endl;
			text.append(key_terms[i].trans[j]);
			if (key_terms[i].trans.size() > 0 && j != (key_terms[i].trans.size() - 1))
				text.append(";"); // translation and query expansion using google and Wikipedia
		}
		if (key_terms[i].mt_tran.length() > 0) {
			if (key_terms[i].trans.size() > 0)
				text.append(":"); // for machine translation
			xmlfile << "\t\t\t\t\t<TRAN TM=\"" << "MT" << "\">" << key_terms[i].mt_tran << "</TRAN>" << endl;
			//xmlfile << "\t\t\t\t<KEYTERM SCORE=\"%0.1f\" TM=<<  << ">%s</KEYTERM>\n ", key_terms[i].score << "MT", key_terms[i].mt_tran.c_str());
			text.append(key_terms[i].mt_tran.c_str());
		}
		if (mt_terms_.size() > 1 && i != (mt_terms_.size() - 1))
			text.append(",");
		xmlfile << "\t\t\t\t</KEYTERM>" << endl;
	}

	xmlfile << "\t\t\t</KEYTERMS>" << endl << "\t\t</QUESTION_ANALYSIS>" << endl;

	file << text;
	file.close();
	xmlfile.close();
}

std::string algorithm::remove_questionstopwords(std::string& question, std::string lang)
{
	icstring a_question(question.c_str());
	//icstring replace = " ";

	int len = 0;
	const char **question_phrase = 0;

	if (lang == "EN" || lang == "en") {
		len = stpl::nlp::question_phrase_english_length;
		question_phrase = stpl::nlp::question_phrase_english;
	}
	else if (lang == "CS" || lang == "cs"
			 || lang == "CT" || lang == "ct"){
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
	return a_question.c_str();
}
