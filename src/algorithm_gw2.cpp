/*
 * algorithm_gw2.cpp
 *
 *  Created on: Feb 16, 2010
 *      Author: monfee
 */

#include "algorithm_gw2.h"

#include <string>
#include <assert.h>
#include <string.h>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <set>
#include <map>
#include <sstream>
#include <vector>

#include "sys_file.h"
#include "ant_link_parts.h"
#include "query.h"
#include "string_utils.h"
#include "language.h"
#include "config.h"
#include "google_translator.h"
#include "application_settings.h"
#include "key_term.h"

#include <stpl/stpl_parser.h>
#include <stpl/stpl_property.h>

#include "uniseg_settings.h"
#include "uniseg.h"
#include "uniseg_types.h"
#include <freq_counter.h>

using namespace std;

namespace NTCIR {

	const char *algorithm_gw2::EMPTY_WIKI_ZH_TERM = "XX$EMPTY";

	struct translation_string_compare
	    : public std::binary_function<string, string, bool>
	{
	    bool operator() (const string& x, const string& y) const {
	    	if (x == y)
	    		return false;
	    	if (x.find(y) != string::npos)
	    		return false;
	    	if (y.find(x) != string::npos)
	    		return false;
	    	return true; //CICOMPARE(x.c_str(), y.c_str());
	    }
	};

	algorithm_gw2::algorithm_gw2()
	{
		string function = application_settings::instance().attributeof("FUNCTION");

		if (function == "TEST1")
			gw2_function_ = GW2_TEST1;
		else if (function == "TEST2")
			gw2_function_ = GW2_TEST2;
		else if (function == "TEST3") // google translator only
			gw2_function_ = GW2_TEST3;
		else if (function == "TEST4") // google translator only
			gw2_function_ = GW2_TEST4;
		else if (function == "WIKI_AND_CLUE2") // google translator only
			gw2_function_ = WIKI_AND_CLUE_V2;
		else if (function == "WIKI_AND_CLUE") // google translator only
			gw2_function_ = WIKI_AND_CLUE;
		else if (function == "WIKI") // google translator only
			gw2_function_ = WIKI_ONLY;

		cerr << "---------------- GW2: " << function << "----------------" << endl;
	}

	algorithm_gw2::~algorithm_gw2()
	{

	}

	void algorithm_gw2::start(std::string& a_question, key_term_array_type& key_terms)
	{
		string question(a_question);

		use_cluetext_translation_ = false;

		if (gw2_function_ == WIKI_AND_CLUE || gw2_function_ == WIKI_AND_CLUE_V2)
			use_cluetext_translation_ = true;

#ifdef DEBUG
#else
//		if (!sys_file::exist(chinese_query_terms_xmlfile_.c_str()) || application_settings::instance().override_cache())
#endif
		{
			gw(question, key_terms);
			if (mt_terms_.size() > 0) {
				string terms = get_mt(mt_terms_);

				string::size_type pos = 0, pos1 = 0;
				int count = 0;
				while ((pos = terms.find(",", pos1)) != string::npos) {
					mt_terms_[count].mt_tran = string(terms, pos1, pos - pos1);
					pos1 = pos;
					++pos1;
					++count;
				}
				mt_terms_[count].mt_tran = string(terms, pos1, terms.length() - pos1);

				for (int i = 0; i < mt_terms_.size(); ++i)
					key_terms[mt_terms_[i].id].mt_tran = mt_terms_[i].mt_tran;
			}
			save_query_terms(key_terms);
		}

		if (gw2_function_ == WIKI_AND_CLUE_V2)
			wiki_and_clue2(question);

		algorithm::start(question, key_terms);

		string chinese_query_terms_xmlfile = cache_path_ +  sys_file::SEPARATOR + CHINESE_QUERY_ANALYSIS_FILE;
		key_term::load_key_terms(chinese_query_terms_xmlfile, query_terms_);

		switch (gw2_function_) {
			case GW2_TEST1:
				test1();
				break;
			case GW2_TEST2:
				test2();
				break;
			case GW2_TEST3:
				test3();
				break;
			case GW2_TEST4:
				test4();
				break;
			case WIKI_AND_CLUE_V2:
//				wiki_and_clue2();
//				break;
			case WIKI_AND_CLUE:
				wiki2();
				break;
			case WIKI_ONLY:
				wiki_only();
				break;
			default:
				test1();
				break;
		}

		std::replace_if(terms_.begin(), terms_.end(), std::bind2nd(std::equal_to<char>(),'-'), ' ') ;
	}

	void algorithm_gw2::gw(std::string& question, key_term_array_type& key_terms)
	{
		// where score of key term equals 1.0
		// all the terms with score 0.99 should be grouped together when search on google
		for (int i = 0; i < key_terms.size(); ++i) {
			int score = (int)(key_terms[i].score * 100);
			int count = language::count_terms(key_terms[i].term.c_str());
			bool acronym = key_terms[i].term.length() > 1 ? (isupper(key_terms[i].term[1]) || key_terms[i].term[1] == '.') : false;
			if (score == 100/* && (count > 1 || acronym)*/) {
				google_query_terms_.push_back(key_term());
				google_query_terms_.back().id = key_terms[i].id;
				google_query_terms_.back().term = (key_terms[i].term);
			}
			else if (score == 99)
				google_query_terms_.back().tip_term.append(key_terms[i].term);
			else {
				mt_terms_.push_back(key_term());
				mt_terms_.back().term.append(key_terms[i].term);
				mt_terms_.back().id = key_terms[i].id;
			}
		}

//		if (google_query_terms_.size() == 0) {
//			string file_name = question;
//			process_filename(file_name);
//			string google_translate_cache_file = create_google_translate_cache_filename(file_name);
//			terms_.append(get_google_translate_and_save_cache(question, google_translate_cache_file));
//			terms_.append(" ");
//		}

		const char *content =  NULL;

		for (int i = 0; i < google_query_terms_.size(); ++i) {
			string google_what("\"");
			bool has_tip_term = false;
			google_what.append(google_query_terms_[i].term + "\"");
			if (google_query_terms_[i].tip_term.length() > 0) {
				has_tip_term = true;
				google_what.append(string(" \"") + google_query_terms_[i].tip_term + "\"");
			}

			string file_name = google_what;
			process_filename(file_name);

			string term_text = google_what;
			trim_non_alnum(term_text);

			string google_translate_cache_file = create_google_translate_cache_filename(file_name);
			google_query_terms_[i].mt_tran = /*key_terms[google_query_terms_[i].id].mt_tran = */get_google_translate_and_save_cache(term_text, google_translate_cache_file);
			if (has_tip_term)
				process_tip_term_mt(google_query_terms_[i].mt_tran, google_query_terms_[i].tip_term_tran);

			string google_cache_file = cache_path_ +  sys_file::SEPARATOR + "google_" + file_name + ".html";
			content = algorithm_gw::google_and_save_cache(google_what, google_cache_file);

			this->process_google_results(content);
			collect_chinese(content);
			if (use_cluetext_translation_) {
				vector<string> term_array;
				term_array.push_back(google_query_terms_[i].term);
				if (google_query_terms_[i].tip_term.length() > 0)
					term_array.push_back(google_query_terms_[i].tip_term);
				get_cluetext_translation(term_array, content);
			}

			if (all_wiki_urls_.size() > 0) {
				// there is an ambiguity, which one to choose
				// there might be one of the link is correct
				// let's find out

				for (int j = 0; j < all_wiki_urls_.size(); ++j) {
					string wiki_zh_url;
					pair<string, pair<string, string> > title_cat;
					string title;
					if (all_wiki_urls_[j].find("File:") != string::npos)
						continue;

					if (all_wiki_urls_[j].find("zh.") == string::npos) {
						title_cat = get_wiki_title(all_wiki_urls_[j]);
						title = title_cat.second.first + (title_cat.second.second.length() > 0 ? string("_") + title_cat.second.second : "");

						string wiki_en_cache_file = this->create_wiki_en_cache_filename(title);
						content =  get_url_and_save_cache(all_wiki_urls_[j], wiki_en_cache_file);

						string en_title = get_wikipedia_page_title(content);

	//					if (strncasecmp(en_title.c_str(), google_query_terms_[i].term.c_str(), google_query_terms_[i].term.length()) == 0) {
	//						if (en_title.length() > google_query_terms_[i].term.length())
	//							break;
	//					}
	//					else {
	//						if (strstr(content, google_query_terms_[i].term.c_str()) == NULL)
	//							break;
	//					}


						wiki_zh_url = process_wikipedia_en_page(content);
					}
					else
						wiki_zh_url = all_wiki_urls_[j];

					if (wiki_zh_url.length() > 0) {
						title_cat = get_wiki_title(wiki_zh_url);
						title = title_cat.second.first + (title_cat.second.second.length() > 0 ? string("_") + title_cat.second.second : "");
						string wiki_zh_cache_file = create_wiki_zh_cache_filename(title);
						content = get_url_and_save_cache(wiki_zh_url, wiki_zh_cache_file);
						string real_title = get_wikipedia_page_title(content);

						//real_title = get_wiki_title(real_title).second.first;
						if (real_title != google_query_terms_[i].mt_tran && strstr(content, google_query_terms_[i].mt_tran.c_str()) != NULL) {
							expansion_terms_.append(google_query_terms_[i].mt_tran + " ");
						}

						//key_terms[google_query_terms_[i].id].trans.push_back(real_title);
						google_query_terms_[i].trans.push_back(real_title);
					}
//					else
//						//key_terms[google_query_terms_[i].id].trans.push_back(EMPTY_WIKI_ZH_TERM);
//						google_query_terms_[i].trans.push_back(EMPTY_WIKI_ZH_TERM);
				}
			}
		}
	}

	void algorithm_gw2::wiki()
	{
		int j = 0;
		while (j < query_terms_.size()) {
			// voting for correct translation,
			// @1 if machine translation is same with the wiki, then 100% sure that is correct translation
			// @2.1 if there is an ambiguity in Wiki, and the translation from MT is different with the Wiki, then choose the on in wiki
			// @2.2 if more than one title in wiki, then use them all
			bool get_mt = true;
			int i = 0;

			string& mt_tran = query_terms_[j].mt_tran;
			if (query_terms_[j].trans.size() > 0) {
				set<string> wiki_title_set;
				for (i = 0; i < query_terms_[j].trans.size(); ++i) {
					string& wiki_tran = query_terms_[j].trans[i];
					if (wiki_tran != EMPTY_WIKI_ZH_TERM) {
						wiki_title_set.insert(wiki_tran);
						get_mt = false;
					}
				}

				if (!get_mt) {
					if (wiki_title_set.find(mt_tran) == wiki_title_set.end())
						for (i = 0; i < query_terms_[j].trans.size(); ++i) {
							string& wiki_tran = query_terms_[j].trans[i];
							if (wiki_tran != EMPTY_WIKI_ZH_TERM)
								wiki_translation_.append(query_terms_[j].trans[i] + " ");
						}
					else
						wiki_translation_.append(mt_tran + " ");
				}
				//						std::ostringstream ostream;
				//						std::copy (wiki_title_set.begin(), wiki_title_set.end(), std::ostream_iterator<string>(ostream, " "));
				//						terms_.append(ostream.str());
			}
			++j;
		}
	}

	void algorithm_gw2::wiki2()
	{
		int j = 0;
		while (j < google_query_terms_.size()) {
			// voting for correct translation,
			// @1 if machine translation is same with the wiki, then 100% sure that is correct translation
			// @2.1 if there is an ambiguity in Wiki, and the translation from MT is different with the Wiki, then choose the on in wiki
			// @2.2 if more than one title in wiki, then use them all
			//bool get_mt = true;
			int i = 0;
			bool agree_mt = false;

			int number_of_terms = language::count_terms(google_query_terms_[j].term.c_str());
			map<string, int, translation_string_compare> votes;
			string& mt_tran = google_query_terms_[j].mt_tran;
			string& tip_term_tran = google_query_terms_[j].tip_term_tran;
			string final_tran;

			if (google_query_terms_[j].trans.size() == 1)
				final_tran = google_query_terms_[j].trans[0];
			else {
//				for (i = 0; i < google_query_terms_[j].trans.size(); ++i) {
//					string& tran =google_query_terms_[j].trans[i];
//					wiki_translation_.append(tran + " ");
//				}
				if (cluetext_translation_.length() > 0)
					final_tran = cluetext_translation_;
				else
					final_tran = mt_tran;
			}

			map<string, int, translation_string_compare>::iterator it;
			for (i = 0; i < google_query_terms_[j].trans.size(); ++i) {
				string& tran =google_query_terms_[j].trans[i];
				votes.insert(make_pair(tran, 1));
			}
			if ((it = votes.find(mt_tran)) != votes.end())
				it->second += 1;
			else
				votes.insert(make_pair(mt_tran, 1));

			if (tip_term_tran.length() > 0)
				if ((it = votes.find(tip_term_tran)) != votes.end())
					it->second++;
				else
					votes.insert(make_pair(tip_term_tran, 1));

			if (cluetext_translation_.length() > 0)
				if ((it = votes.find(cluetext_translation_)) != votes.end())
					it->second++;
				else
					votes.insert(make_pair(cluetext_translation_, 1));

			it = votes.begin();
			int max = it->second;
			string voted_tran = it->first;
			++it;
			for (; it != votes.end(); ++it) {
				if (it->second > max) {
					max = it->second;
					voted_tran = it->first;
				}
			}

			if (max > 1) {
				final_tran = voted_tran;

				//if (google_query_terms_[j].trans.size() == 1 && final_tran)
			}

			if ( (machine_translation_.find(final_tran) == string::npos )
				|| (tip_term_tran.length() > 0 && tip_term_tran != final_tran) ) {
				use_pruned_translation_ = true;

				if (max == 1 && final_tran == mt_tran)
					use_pruned_translation_ = false;
			}

			if (google_query_terms_[j].trans.size() == 1 && final_tran != google_query_terms_[j].trans[0])
				wiki_translation_.append(google_query_terms_[j].trans[0] + " ");
//
//
//			set<string> wiki_title_set;



//			for (i = 0; i < google_query_terms_[j].trans.size(); ++i) {
//				string& wiki_tran = google_query_terms_[j].trans[i];
//				if (wiki_tran != EMPTY_WIKI_ZH_TERM)
//					wiki_title_set.insert(wiki_tran);
//			}
//
//			if (wiki_title_set.find(mt_tran) == wiki_title_set.end())
//				for (i = 0; i < google_query_terms_[j].trans.size(); ++i) {
//					string& wiki_tran = google_query_terms_[j].trans[i];
//					if (wiki_tran != EMPTY_WIKI_ZH_TERM) {
//						wiki_translation_.append(wiki_tran + " ");
//						if (final_wiki_tran)
//						final_wiki_tran = wiki_tran;
//					}
//				}
//					else {
//						// replace the
//						//wiki_translation_.append(mt_tran + " ");
//						final_tran = mt_tran;
//					}
				//						std::ostringstream ostream;
				//						std::copy (wiki_title_set.begin(), wiki_title_set.end(), std::ostream_iterator<string>(ostream, " "));
				//						terms_.append(ostream.str());
//				if (final_wiki)


			if (gw2_function_ == WIKI_AND_CLUE_V2) {
				string id("^ ");
				id.append(number_to_string(google_query_terms_[j].id));
				string::size_type pos = machine_translation_pruned_.find(id);
				if (pos != string::npos)
					machine_translation_pruned_.replace(pos, id.length(), final_tran);
				string replace("");
				find_and_replace(wiki_translation_, final_tran, replace);
			}
//			}
			++j;
		}
	}

	void algorithm_gw2::get_cluetext_translation(vector<string>& term_array, const char *content)
	{
		//string temp_term = term;
		// vector<string> term_array;
//		string find = "\"";
//		string replace = "";
//		const char *current = term.c_str();

//		while (*current != '\0') {
//			while (*current != '\0' && !isalpha(*current))
//				++current;
//
//			if (*current == '\0')
//				break;
//
//			term_array.push_back(string(""));
//			while (*current != '\0' && (*current == ' '|| isalpha(*current)) || *current == '-') {
//				term_array.back().push_back(*current);
//				++current;
//			}
//			find_and_replace(term_array.back(), find, replace);
//			//cerr << "search clue text translation for (" << term_array.back() << ")" << endl;
//		}

		//string copy(content);
		//string_clean_tag(copy);
		//cerr << content << endl;

		const char *where = NULL, *start = content/*copy.c_str()*/, *tran_start = NULL, *tran_end = NULL;
		bool found = false;
		for (int i = 0; i < term_array.size(); ++i) {
			while ((where = strcasestr(start, term_array[i].c_str())) != NULL) {
				where += term_array[i].length();

				while (isspace(*where))
					++where;

				if (*where == '<') {			// then remove the XML tags
					while (*where != '\0' && *where != '>')
						++where;
					++where;
				}

				while (isspace(*where))
					++where;

				if (*where != '\0')
					if (*where == '('/* || *where == '-'*/) {
						 ++where;
						while (isspace(*where))
							++where;
						tran_start = tran_end = where;
						while (language::ischinese(where))
							tran_end = where += language::utf8_bytes(where);

						if (tran_end > tran_start) {
							cluetext_translation_ = string(tran_start, tran_end);
							break;
						}
					}
//					bracket_start
//				if ((bracket_start = strstr(where, "(")) != NULL) {
//					bracket_end = strstr(bracket_start, "(");
//				}

				start = where;
			}
			if (cluetext_translation_.length() > 0)
				break;
		}
	}

	void algorithm_gw2::wiki_and_clue2(std::string& a_question)
	{
		string question(a_question);
		string::size_type pos;
		for (int i = 0; i < google_query_terms_.size(); ++i) {
			pos = question.find(google_query_terms_[i].term);
			if (pos != string::npos) {
				string id("\"^");
				id.append(number_to_string(google_query_terms_[i].id) + "\"");
				question.replace(pos, google_query_terms_[i].term.length(), id);
				if (google_query_terms_[i].tip_term.length() > 0) {
					pos = question.find(google_query_terms_[i].tip_term);
					if (pos != string::npos)
						question.replace(pos, google_query_terms_[i].tip_term.length(), "");
				}
			}
		}
		while (1) {
			if ((pos = question.find("\"\"")) != string::npos)
				question.replace(pos, 2, "\"");
			else
				break;
		}
		get_mt_question(question, machine_translation_pruned_);
		find_and_replace(machine_translation_pruned_, string("\""), string(""));
		//a_question = question;
	}

	void algorithm_gw2::process_tip_term_mt(std::string& mt_tran, std::string& tip_term_tran)
	{
		const char *current = mt_tran.c_str();
		while (!language::ischinese(current))
			current += language::utf8_bytes(current);

		mt_tran.replace(0, current - mt_tran.c_str(), "");
		current = mt_tran.c_str();
		while (language::ischinese(current))
			current += language::utf8_bytes(current);

		tip_term_tran = mt_tran.substr(current - mt_tran.c_str());
		mt_tran.replace(current - mt_tran.c_str(), string::npos , "");

		current = tip_term_tran.c_str();
		while (!language::ischinese(current))
			current += language::utf8_bytes(current);
		tip_term_tran.replace(0, current - tip_term_tran.c_str(), "");
		current = tip_term_tran.c_str();
		while (language::ischinese(current))
			current += language::utf8_bytes(current);
		if (!current != '\0')
			tip_term_tran.replace(tip_term_tran.begin() + (current - mt_tran.c_str()), tip_term_tran.end(), "");
	}

	void algorithm_gw2::wiki_only()
	{
		wiki();
//		if (terms_.length() > 0 && terms_[terms_.length() - 1] != ' ')
//			terms_.append(" ");
//		terms_.append(wiki_translation_);
//		if (gw2_function_ == WIKI_AND_CLUE_V2)
//			for (int i = 0; i < google_query_terms_.size(); ++i) {
//				string id("^ ");
//				id.append(number_to_string(google_query_terms_[i].id));
//				string::size_type pos = machine_translation_.find(id);
//				if (pos != string::npos)
//					machine_translation_.replace(pos, id.length(), "");
//			}
	}

	void algorithm_gw2::test1()
	{
		int j = 0;
		while (j < query_terms_.size()) {
			// voting for correct translation,
			// @1 if machine translation is same with the wiki, then 100% sure that is correct translation
			// @2.1 if there is an ambiguity in Wiki, and the translation from MT is different with the Wiki, then choose the on in wiki
			// @2.2 if more than one title in wiki, then use them all
			bool get_mt = false;
			int i = 0;
			if (terms_.length() > 0 && terms_[terms_.length() - 1] != ' ')
				terms_.append(" ");

			if (query_terms_[j].trans.size() > 0) {
				set<string> wiki_title_set;
				for (i = 0; i < query_terms_[j].trans.size(); ++i)
					if (query_terms_[j].trans[i] != EMPTY_WIKI_ZH_TERM) {
						wiki_title_set.insert(query_terms_[j].trans[i]);
						get_mt = false;
					}
					else
						get_mt = true;

				if (!get_mt) {
					if (wiki_title_set.find(query_terms_[j].mt_tran) != wiki_title_set.end())
						terms_.append(query_terms_[j].mt_tran);
					else {
//						std::ostringstream ostream;
//						std::copy (wiki_title_set.begin(), wiki_title_set.end(), std::ostream_iterator<string>(ostream, " "));
//						terms_.append(ostream.str());
						for (i = 0; i < query_terms_[j].trans.size(); ++i)
							if (query_terms_[j].trans[i] != EMPTY_WIKI_ZH_TERM)
								terms_.append(query_terms_[j].trans[i] + " ");
					}
				}
			} else
				get_mt = true;

			if (get_mt && query_terms_[j].mt_tran.length() > 0)
				terms_.append(query_terms_[j].mt_tran);
			++j;
		}


	}

	void algorithm_gw2::test2()
	{
		// put the category in
		test1();

		if (terms_[terms_.length()] != ' ')
			terms_.append(" ");

		terms_.append(machine_translation_);
	}

	void algorithm_gw2::test3()
	{

	}

	void algorithm_gw2::test4()
	{

	}

	void algorithm_gw2::wiki_and_clue2()
	{
		wiki_and_clue();
	}

	void algorithm_gw2::wiki_and_clue()
	{
		wiki_only();

//		if (cluetext_translation_.length() > 0) {
//			terms_.append(" ");

	}
}
