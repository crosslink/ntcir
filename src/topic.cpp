/*
 * topic.cpp
 *
 *  Created on: Dec 27, 2009
 *      Author: monfee
 */

#include "topic.h"
#include "search_engine_ant.h"
#include "corpus_wikipedia.h"
#include "application_settings.h"
#include "sys_file.h"
#include "algorithm_gw.h"
#include "algorithm_gw2.h"
#include "algorithm_question_phrase_chinese.h"
#include "corpus_target.h"

#include <iostream>
#include <set>

#include <assert.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

using namespace std;

namespace NTCIR {

	topic::topic(std::string& id, std::string& title) :
			id_(id), title_(title), file_(stdout)
	{
		docids_ = NULL;
		hits_ = 0;
		number_to_list_ = 10;

		char *cwd = get_current_dir_name();
		topic_cache_path_ = string(cwd) + string(sys_file::SEPARATOR) + application_settings::instance().get_cache_path() + string(sys_file::SEPARATOR) + id_;
		free(cwd);

		string::size_type pos1 = id_.find("-");
		string::size_type pos2 = id_.find("-", ++pos1);
		dest_lang_ = string(id_, pos1, pos2 - pos1);

		create_topic_cache_path();
		set_algorithm();
	}

	topic::~topic()
	{

	}

	void topic::create_topic_cache_path()
	{
		if (!sys_file::exist(topic_cache_path_.c_str()))
			sys_file::create_directory((topic_cache_path_).c_str());
	}

	void topic::retrieve_en(const char *run_id)
	{
		//algorithm_gw gw(topic_cache_path_);
		//gw.start(question_en_);
//		if (question_en_.length() == 0)
//			algorithm_->start(key_terms_);
//		else
//			algorithm_->start(question_en_);
		if (algorithm_) {
			algorithm_->start(question_en_, key_terms_);

			const string& terms = algorithm_->get_terms();

			if (terms.length() == 0)
				cerr << "Topic: " << id_ << ", can't find any Chinese terms." << endl;
			else
				search(terms.c_str(), run_id);
		} else
			cerr << "No algorithm specified!" << endl;
		//corpus_wikipedia::
	}

	void topic::retrieve_zh(const char *run_id)
	{
		search(question_zh_.c_str(), run_id);
	}

	void topic::retrieve(const char *run_id)
	{
		const char *pos = strchr(run_id, '-');
		if (pos) {
			++pos;
			src_lang_ = string(pos, pos + 2);
			pos = strchr(pos, '-');
			if (pos) {
				++pos;
				dest_lang_ = string(pos, pos + 2);
			}
		}

		if (algorithm_) {
			algorithm_->set_dest_lang(dest_lang_);
			algorithm_->set_src_lang(src_lang_);
			if (src_lang_ == "EN")
				algorithm_->start(question_en_, key_terms_);
			else
				algorithm_->start(question_zh_, key_terms_);

			const string& terms = algorithm_->get_terms();

			if (terms.length() == 0)
				cerr << "Topic: " << id_ << ", can't find any Chinese terms." << endl;
			else
				search(terms.c_str(), run_id);
		}
	}

	void topic::search(const char *terms, const char *run_id)
	{
		cerr << "[" << id_ << "]Search with terms:" << terms << endl << endl;

//		docids_ = search_engine_ant::instance().search(terms);;
//		hits_ = search_engine_ant::instance().hits();
		//hits_ = corpus_target::instance().search(terms, docids_);
		string output_topic_header = application_settings::instance().attributeof("output_topic_header");
		string output_topic_footer = application_settings::instance().attributeof("output_topic_footer");
		string output_document = application_settings::instance().attributeof("output_document");

		docids_ = corpus_target::instance().search(terms, &hits_);

		if (hits_ == 0) {
			cerr << "[" << id_ << "] No documents found" << endl;
			return;
		}

		printf(output_topic_header.c_str(), id_.c_str());

		long long last_to_list = hits_ > number_to_list_ ? number_to_list_ : hits_;

		long long which = 0;
		std::set<string> docid_set;
		for (; which < last_to_list; which++) {
			if (docid_set.find(docids_[which]) == docid_set.end()) {
				docid_set.insert(docids_[which]);
				printf(output_document.c_str(), which + 1, docids_[which], (last_to_list - which));
			}
		}
			//fprintf(file_, "%s Q0 %s %lld %lld %s\n", id_.c_str(), docids_[which], which + 1, (last_to_list - which), run_id);

		puts(output_topic_footer.c_str());
	}

    void topic::add_key_term(std::string term, float score)
    {
    	int pre_score = key_terms_.size() > 0 ? (int)(key_terms_.back().score * 10) : 0;
    	int curr_score = score * 10;
    	if (pre_score == 6 && curr_score != 10)
    		key_terms_.back().term.append(" " + term);
    	else {
			key_terms_.push_back(key_term());
			key_terms_.back().term = term;
			key_terms_.back().id = key_terms_.size() - 1;
    	}
		key_terms_.back().score = score;
    }

    void topic::set_algorithm()
    {
    	std::string algor_name = application_settings::instance().attributeof("algorithm");
    	if (algor_name == "GW1")
    		algorithm_ = new algorithm_gw();
    	else if (algor_name == "GW2")
    		algorithm_ = new algorithm_gw2();
    	else if (algor_name == "QT")
    		algorithm_ = new algorithm_question_phrase_chinese();
    	else
    		algorithm_ = new algorithm();

    	algorithm_->set_cache_path(topic_cache_path_);
    }
}
