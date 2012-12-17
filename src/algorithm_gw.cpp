/*
 * algorithm_gw.cpp
 *
 *  Created on: Jan 3, 2010
 *      Author: monfee
 */

#include "algorithm_gw.h"

#include <string>
#include <iostream>
#include <vector>
#include <set>

#include <assert.h>
#include <string.h>
#include <stdexcept>

#include "sys_file.h"
#include "ant_link_parts.h"
#include "query.h"
#include "string_utils.h"
#include "language.h"
#include "config.h"
#include "google_translator.h"
#include "application_settings.h"

#include <stpl/stpl_parser.h>
#include <stpl/stpl_property.h>

#include "uniseg_settings.h"
#include "uniseg.h"
#include "uniseg_types.h"
#include <freq_counter.h>

using namespace std;

namespace NTCIR {

	const char *algorithm_gw::GOOGLE_CACHE_FILE1 = "google1.html";
	const char *algorithm_gw::GOOGLE_CACHE_FILE2 = "google2.html";
	const char *algorithm_gw::WIKIPEDIA_CACHE_FILE_EN = "wikipedia_en.html";
	const char *algorithm_gw::WIKIPEDIA_CACHE_FILE_ZH = "wikipedia_zh.html";
	const char *algorithm_gw::CHINESE_TEXT_FILE = "chinese.txt";
	const char *algorithm_gw::GOOLGE_TRANSLATION_TEXT_FILE = "google_translate.txt";

	const char *algorithm_gw::ALGORITHM_GW_CONFIG_FILE = "algorithm_gw.conf";

	algorithm_gw::algorithm_gw()
	{
		google_cache_file1_ = cache_path_ +  sys_file::SEPARATOR + GOOGLE_CACHE_FILE1;
		google_cache_file2_ = cache_path_ +  sys_file::SEPARATOR + GOOGLE_CACHE_FILE2;
		wikipedia_cache_file_en_ = cache_path_ +  sys_file::SEPARATOR + WIKIPEDIA_CACHE_FILE_EN;
		wikipedia_cache_file_zh_ = cache_path_ +  sys_file::SEPARATOR + WIKIPEDIA_CACHE_FILE_ZH;
		chinese_text_file_ = cache_path_ +  sys_file::SEPARATOR + CHINESE_TEXT_FILE;
		google_translate_text_file_ = cache_path_ +  sys_file::SEPARATOR + GOOLGE_TRANSLATION_TEXT_FILE;

		set_locale_ = false;
		with_translation_= false;

		if (sys_file::exist(ALGORITHM_GW_CONFIG_FILE))
			config_ = new config(ALGORITHM_GW_CONFIG_FILE);
	}

	algorithm_gw::~algorithm_gw()
	{
	}

	void algorithm_gw::start(std::string& question, key_term_array_type& key_terms)
	{
		std_tolower(question);

//		string numbers;
//		string number;
//		for (int i = 0; i < question.length(); i++) {
//			if (isdigit(question[i])) {
//				while (isdigit(question[i])) {
//					number.push_back(question[i]);
//					++i;
//				}
//				numbers.append(number + " ");
//				number.clear();
//			}
//		}

		if (config_ != NULL) {
			string function = application_settings::instance().attributeof("FUNCTION");

			if (function == "TEST1")
				test1(question, langpair_.c_str());
			else if (function == "TEST2")
				test2(question, langpair_.c_str());
			else if (function == "TEST3") // google translator only
				test3(question, langpair_.c_str());
			else if (function == "TEST4") // google translator only
				test4(question, langpair_.c_str());
			else if (function == "TEST5") // google translator only
				test5(question, langpair_.c_str());
			else if (function == "TEST6") // google translator only
				test6(question, langpair_.c_str());
		}
		else
			test1(question, "en|zh");

//		terms_.append(" ");
//		terms_.append(numbers);
		for (int i = 0; i < terms_.length(); i++)
			if (terms_[i] == ',')
				terms_[i] = ' ';
	}

	void algorithm_gw::gw(std::string& question)
	{
		char *content =  NULL;
		string wiki_en_url;
		bool has_wiki_url = false;
		wiki_url_.clear();
		all_wiki_urls_.clear();

		//query new_query;
		if (!sys_file::exist(wikipedia_cache_file_zh_.c_str())) {

			//if (!sys_file::exist(chinese_text_file_.c_str())) {
				if (!sys_file::exist(wikipedia_cache_file_en_.c_str())) {
					if (sys_file::exist(google_cache_file2_.c_str()))
						content = query::retrieve_localfile(google_cache_file2_.c_str());
					else {
						if (!sys_file::exist(google_cache_file1_.c_str())) {
							if (set_locale_)
								content = query::google_site(question.c_str(), locale_.c_str(), true);
							else
								content = query::google(question.c_str(), true);
							write_cache(content, google_cache_file1_);
						}
						else
							content = query::retrieve_localfile(google_cache_file1_.c_str());

						if (!(has_wiki_url = process_google_results(content))) {
							string terms;
							get_google2_terms(terms);
							content = query::google_site(terms.c_str(), "en.wikipedia.org");
							write_cache(content, google_cache_file2_);
						}
					}
					if (!has_wiki_url)
						process_google2_results(content, question); // changed to google2 processing after rev.609

					if(get_wiki_url().length() > 0) {
						content = query::retrieve_webpage(get_wiki_url().c_str());
						write_cache(content, wikipedia_cache_file_en_);
					}
					else
						content = NULL;
				}
				else
					content = query::retrieve_localfile(wikipedia_cache_file_en_.c_str());

				if (content) {
					string wiki_zh_url = process_wikipedia_en_page(content);

					if (wiki_zh_url.length() > 0) {
						string new_wiki_zh_url;
						if (wiki_zh_url.find_last_of("%") != string::npos) {
							string::size_type pos = wiki_zh_url.find_last_of("/");
							new_wiki_zh_url = string(wiki_zh_url, 0, pos + 1);
							string chinese_title(wiki_zh_url, pos + 1);
							char buf[PATH_MAX];
							convert_encoded_unicode(buf, chinese_title);
							new_wiki_zh_url.append(buf);
						}
						else
							new_wiki_zh_url = wiki_zh_url;

						content = query::retrieve_webpage(new_wiki_zh_url.c_str());
						write_cache(content, wikipedia_cache_file_zh_);
					}
					else
						content = NULL;
				}
//			}
//			else {
//				content = query::retrieve_localfile(chinese_text_file_.c_str());
//				chinese_terms_ = content;
//				content = NULL;
//			}

		}
		else {
//			string local_file_url("FILE://");
//			local_file_url.append(wikipedia_cache_file_zh_);

			content = query::retrieve_localfile(wikipedia_cache_file_zh_.c_str());
		}

		if (content)
			terms_ = get_wikipedia_page_title(content);

		collect_chinese(NULL);
	}

	std::pair<std::string, std::pair<std::string, std::string> > algorithm_gw::get_wiki_title(std::string& wiki_url)
	{
		char buf[PATH_MAX];
		static const char *cat_sep1 = "_(";
		static const char *cat_sep2 = ")";
		string::size_type pos = wiki_url.find_last_of("/");
		string new_wiki_url(wiki_url, 0, pos + 1);
		string category; // category
		string title(wiki_url, pos + 1);
		if (wiki_url.find_last_of("%") != string::npos) {
			convert_encoded_unicode(buf, title);
			new_wiki_url.append(buf);
			title = buf;
		} else
			new_wiki_url = wiki_url;
		pos = title.find(cat_sep1);
		if (pos != string::npos) {
			string::size_type pos1 = pos + strlen(cat_sep1);
			string::size_type pos2 = title.find(cat_sep2, pos);
			category = string(title, pos1, pos2 - pos1);
			title.replace(pos, string::npos, "");
		}
		return make_pair(new_wiki_url, make_pair(title, category));
	}

	void algorithm_gw::get_google_translate(std::string& question, string& terms, const char *language_pair) {
		if (/*terms_.length() == 0 && */strlen(language_pair) > 0) {

			if (!sys_file::exist(google_translate_text_file_.c_str())) {
				string stran = question;
				language::remove_en_stopword(stran, ",");
				terms = google_translator().translate(stran.c_str(), language_pair);

				write_cache(terms.c_str(), google_translate_text_file_);
			}
			else {
				terms = query::retrieve_localfile(google_translate_text_file_.c_str());

				if (terms.length() == 0)
					cerr << "Enpty google translate file: " << google_translate_text_file_ << endl;
			}

			language::remove_xml_entities(terms);
		}
	}

	void algorithm_gw::test1(std::string& question, const char *language_pair)
	{
		gw(question);

		if (chinese_terms_.length() > 0) {
			terms_.append(" ");
			terms_.append(chinese_terms_);
		}
		trim_left(terms_, " ");

		if (with_translation_ || terms_.length() == 0) {
			if (terms_.length() > 0)
				terms_.append(" ");

			get_google_translate(question, trans_, language_pair);
			terms_.append(trans_);
		}
	}

	/* use segmentation to search */
	void algorithm_gw::test2(std::string& question, const char *language_pair)
	{
		test1(question, language_pair);

		string s_terms;
		language::collect_chinese(s_terms, terms_.c_str(), true);

		segment_terms(terms_);
		terms_.append(" ");
		terms_.append(s_terms);
	}

	void algorithm_gw::test3(std::string& question, const char *language_pair)
	{
		gw(question);

		string freq_terms = cal_chinese_terms_freq(question, language_pair);

//		chinese_terms_.append(" " + trans);
//
//		terms_.append(" ");
//		terms_.append(chinese_terms_);
		terms_.append(" ");
		terms_.append(freq_terms);
	}

	void algorithm_gw::test4(std::string& question, const char *language_pair)
	{
		gw(question);

		string freq_terms = cal_chinese_terms_freq(question, language_pair);

		segment_terms(terms_);
		//trim_left(terms_, " ");
		terms_.append(" ");
		terms_.append(freq_terms);

		string s_terms;
		language::collect_chinese(s_terms, terms_.c_str(), true);

		terms_.append(" ");
		terms_.append(s_terms);
	}

	void algorithm_gw::test5(std::string& question, const char *language_pair)
	{
		set_locale_ = true;
		with_translation_ = true;
		test2(question, language_pair);
	}

	void algorithm_gw::test6(std::string& question, const char *language_pair)
	{
		set_locale_ = true;
		//with_translation_ = true;
		test4(question, language_pair);
	}

	std::string algorithm_gw::cal_chinese_terms_freq(std::string& question, const char *language_pair)
	{
		if (trans_.length() == 0) {
			get_google_translate(question, trans_, language_pair);
			terms_.append(" ");
			terms_.append(trans_);
		}

		//since we use the frequency analysis so we don't segment the chinese terms
		// now we do

		trim_left(chinese_terms_, " ");
		string freq_terms = chinese_terms_ + " " + trans_;
		string terms;
		if (chinese_terms_.length() > 0) {
			segment_terms(freq_terms);
			cerr << "segmented freq terms: " << freq_terms << endl;

			language::term_freq_map_type term_map;
			language::collect_chinese(term_map, freq_terms.c_str());

			get_chinese_freq_terms(terms, term_map);
			if (terms.length() == 0)
				cerr << "no freq terms found." << endl;
			else
				cerr << "found freq terms: " << terms << endl;
		}
		freq_terms = terms;
		return freq_terms;
	}

	void algorithm_gw::segment_terms(std::string& terms) {
		uniseg_encoding *enc = uniseg_encoding_factory::instance().get_encoding();

		UNISEG_settings::instance().max = 11;
		static UNISEG_uniseg uniseg;

		const char *output = (const char *)uniseg.do_segmentation((unsigned char *)terms.c_str(), terms.length());
		if (strlen(output) < terms.length())
			cerr << ("Warning: length of output is less than the terms' length, output: ") << output << endl;
		terms = output;
	}

	void algorithm_gw::create_term_set(const char *source)
	{
		char buffer[1024];
		char *copy = strdup(source);
		char **term_list = new char *[strlen(copy)];

		string_tolower(copy);
		long count = string_to_list(copy, term_list);

		for (int i = 0; i < count; i++) {
			/**
			 * TODO remove the document format terms, "PDF", "DOC", ...
			 */
			strcpy(buffer, term_list[i]);
			std::map<std::string, int>::iterator it;
			string term(buffer);

			if (language::isstopword(term.c_str()))
				continue;

			if (term == "wikipedia"
					|| term == "free"
							|| term == "encyclopedia")
				continue;

			if ((it = term_set_.find(term)) != term_set_.end())
				it->second++;
			else
				term_set_.insert(make_pair(term, 1));
		}

		delete [] term_list;
		free(copy);
	}

	bool algorithm_gw::process_google_results(const char *content)
	{
		char *start, *end, *link_start, *link_end, *tmp_link_start;
		char link_buffer[1024 * 10];
		int buffer_len = 0;
		bool a_link = false;
		bool found_wiki = false;

		wiki_url_.clear();
		all_wiki_urls_.clear();

		//start = strstr(content, "<li>");
		start = (char *)content;
		while ((start = strstr((char *)start, "<li")) != NULL/* || (start = strstr((char *)start, "<li ")) != NULL*/) {
			start += 3;
			if (*start == 'n')
				continue;

			start = strchr(start, '>');
			assert(start != NULL);

			++start;

			if (!(end = strstr(start, "</li>"))) {
				end = start;
				while ((end = strstr(end, "<li")) && *(end + 3) == 'n') {
					break;
				}
				if (!end)
					break;
			}

			tmp_link_start = start;
			if (tmp_link_start != NULL && (link_start = strstr(tmp_link_start, "<a")) != NULL && link_start < end) {
				++link_start;
				if (*link_start == 'a') {
					a_link = true;
					++link_start;
				}
				else
					link_start += 4;

				link_end = strchr(link_start, '>');
				assert(link_end != NULL);

				stpl::GeneralParser<stpl::Property<string, char *> > property_parser(link_start, link_end);
				property_parser.parse();
				stpl::Property<string, char *> *property;
				while ((property = property_parser.get_next_entity()) != NULL)
					if (property->name() == "href") {
						string::size_type pos;
						string href = property->value();
						string a_wiki_url;
						if ((pos = href.find(".wikipedia.")) != string::npos) {
							pos = 0;
							while (pos < href.length() && (href[pos] != '"' && href[pos] != '&')) {
								a_wiki_url.push_back(href[pos]);
								++pos;
							}
							found_wiki = true;
							if (wiki_url_.length() == 0)
								wiki_url_ = a_wiki_url;

							all_wiki_urls_.push_back(a_wiki_url);
						}
					}

				if (a_link) {
					link_start = ++link_end;
					link_end = strstr(link_end, "</a");
					if (!link_end)
						break;
						//link_end = end;

					buffer_len = link_end - link_start;
					strncpy(link_buffer, link_start, buffer_len);
					link_buffer[buffer_len] = '\0';
					string_strip_tags(link_buffer);

					create_term_set(link_buffer);
					a_link = false;
				}
				tmp_link_start = link_end;
			}

			start = end;
		}
//		else
//			cerr << "No result found in google search page" << endl;
		return found_wiki;
	}

	bool algorithm_gw::process_google2_results(const char *content, std::string& question)
	{
		char *start, *end, *link_start, *link_end, *tmp_link_start;
		char link_buffer[1024 * 10];
		int buffer_len = 0;
		bool a_link = false;

		wiki_url_.clear();

		start = (char *)content;
		while ((start = strstr((char *)start, "<li")) != NULL/* || (start = strstr((char *)start, "<li ")) != NULL*/) {
			start += 3;
			if (*start == 'n')
				continue;

			start = strchr(start, '>');
			assert(start != NULL);

			++start;

			if (!(end = strstr(start, "</li>"))) {
				end = start;
				while ((end = strstr(end, "<li")) && *(end + 3) == 'n') {
					break;
				}
				if (!end)
					break;
			}

			tmp_link_start = start;
			if (tmp_link_start != NULL && (link_start = strstr(tmp_link_start, "<a")) != NULL && link_start < end) {
				++link_start;
				if (*link_start == 'a') {
					a_link = true;
					++link_start;
				}
				else
					link_start += 4;

				link_end = strchr(link_start, '>');
				assert(link_end != NULL);

				stpl::GeneralParser<stpl::Property<string, char *> > property_parser(link_start, link_end);
				property_parser.parse();
				stpl::Property<string, char *> *property;
				while ((property = property_parser.get_next_entity()) != NULL)
					if (property->name() == "href") {
						string::size_type pos;
						string href = property->value();
						if ((pos = href.find("http://en.wikipedia")) != string::npos) {
							wiki_url_.clear();
							while (pos < href.length() && (href[pos] != '"' && href[pos] != '&')) {
								wiki_url_.push_back(href[pos]);
								++pos;
							}
						}
					}

				if (a_link) {
					link_start = start;
					link_end = end;
					if (!link_end)
						break;
						//link_end = end;

					buffer_len = link_end - link_start;
					strncpy(link_buffer, link_start, buffer_len);
					link_buffer[buffer_len] = '\0';
					string_strip_tags(link_buffer);

					term_set_.clear();
					create_term_set(link_buffer);
					std::map<std::string, int>::iterator it = term_set_.begin();
					bool found = false;
					for (; it != term_set_.end(); ++it) {
						if (question.find(it->first) != string::npos) {
							found = true;
							break;
						}
					}
					if (found)
						return true;

					a_link = false;
				}
				tmp_link_start = link_end;
			}


			start = end;
		}
		return false;
	}

	void algorithm_gw::get_google2_terms(std::string& terms, int threshold)
	{
		std::map<std::string, int>::iterator it = term_set_.begin();
		//cerr << "Total google2 terms count: " << term_set_.size() << endl;
		for (; it != term_set_.end(); ++it)
			// debug
			//cerr << it->first << ": " << it->second << endl;
			if (it->second > threshold) {
				if (terms.length() > 0)
					terms.append(" ");
				terms.append(it->first);
			}
		if (terms.size() == 0 && threshold > 0)
			get_google2_terms(terms, threshold - 1);
	}

	void algorithm_gw::get_chinese_freq_terms(std::string& terms, language::term_freq_map_type& term_set, int term_count, int threshold)
	{
//		Freq::freq_type& term_set = freq.set();
//		Freq::freq_type::iterator it = term_set.begin();
		language::term_freq_map_type::iterator it = term_set.begin();
		//cerr << "Total google2 terms count: " << term_set.size() << endl;
		for (; it != term_set.end(); ++it)
			// debug
			//cerr << it->first << ": " << it->second << endl;
			if (it->second > threshold) {
				if (terms.length() > 0)
					terms.append(" ");
				if (terms.find(it->first) == string::npos) {
					terms.append(it->first);
					++term_count;
				}
			}

		if (term_count < 2  && threshold > 0)
			get_chinese_freq_terms(terms, term_set, term_count, threshold - 1);
	}

	void algorithm_gw::get_chinese_freq_terms(std::string& terms, Freq& freq,  int threshold)
	{
		Freq::freq_type& term_set = freq.set();
		Freq::freq_type::iterator it = term_set.begin();
		//cerr << "Total google2 terms count: " << term_set.size() << endl;
		for (; it != term_set.end(); ++it)
			// debug
			//cerr << it->first << ": " << it->second << endl;
			if (it->second->size() > 1 && it->second->freq() > threshold) {
				if (terms.length() > 0)
					terms.append(" ");
				terms.append(it->first);
			}
		if (terms.size() == 0 && threshold > 1)
			get_chinese_freq_terms(terms, freq, threshold - 1);
	}

	std::string algorithm_gw::process_wikipedia_en_page(const char *content) {
		string wiki_zh_url;
		const char *start = content, *end;
		while (start = strstr((char *)start, "http://zh.wikipedia")) {
			end = strchr(start, '"');
			if (wiki_zh_url.length() == 0) {
				wiki_zh_url = string(start, end);
				start = end;
			}
			else {
				const char *tag_end = strchr(end, '>');
				const char *tag_start = strchr(end, '<');
				string chinese(++tag_end, tag_start);
				if (chinese == "\344\270\255\346\226\207")
					wiki_zh_url = string(start, end);
				start = tag_start;
			}
		}

		if (wiki_zh_url.length() == 0) {
			cerr << "No Chinese link in English Wikipedia page" << endl;
			return "";
		}

		static const char *wiki_path = "/wiki/";
		static const char *wiki_path_tw = "/zh-tw/";
		static const char *wiki_path_cn = "/zh-cn/";

		string::size_type pos = wiki_zh_url.find(wiki_path);
		if (pos != string::npos) {
			if (application_settings::instance().is_ct())
				wiki_zh_url.replace(pos, strlen(wiki_path), wiki_path_tw);
			else
				wiki_zh_url.replace(pos, strlen(wiki_path), wiki_path_cn);
		}

		string new_wiki_zh_url;
		if (wiki_zh_url.find_last_of("%") != string::npos) {
			string::size_type pos = wiki_zh_url.find_last_of("/");
			new_wiki_zh_url = string(wiki_zh_url, 0, pos + 1);
			string chinese_title(wiki_zh_url, pos + 1);
			char buf[PATH_MAX];
			convert_encoded_unicode(buf, chinese_title);
			new_wiki_zh_url.append(buf);
		}
		else
			new_wiki_zh_url = wiki_zh_url;

		return new_wiki_zh_url;
	}

	std::string algorithm_gw::get_wikipedia_page_title(const char *content) {
		string terms;
		char *start, *end;
		const char *title_tag = "<title>";
		start = strstr((char *)content, title_tag);
		assert(start != NULL);

		start += strlen(title_tag);
		end = strchr(start, '-');
		--end;
		while (isspace(*end))
			--end;
		++end;
		terms.append(string(start, end));

		string::size_type pos = terms.find(" (");
		if (pos != string::npos)
			terms.replace(pos, string::npos, "");
		return terms;
	}

	void algorithm_gw::collect_chinese(const char *content, bool forceread)
	{
		if (content != NULL) {
			 if( chinese_terms_.length() > 0 && !forceread)
				 return;
			// collect possibble Chinese
			chinese_terms_.clear();
			language::collect_chinese_from_xml_entities(chinese_terms_, content);
			trim_left(chinese_terms_, " ");
			if (chinese_terms_.length() > 0)
				write_cache(chinese_terms_.c_str(), chinese_text_file_);
		}
		else {
			if (sys_file::exist(google_cache_file1_.c_str())) {
				char *file = query::retrieve_localfile(google_cache_file1_.c_str());
				if (file != NULL)
					collect_chinese(file);
			}
		}
	}

	const char *algorithm_gw::google_and_save_cache(std::string& term, std::string& filename) {
		const char *content = NULL;
		if (!sys_file::exist(filename.c_str()) || application_settings::instance().override_cache()) {
			if (set_locale_)
				content = query::google_site(term.c_str(), locale_.c_str(), true);
			else
				content = query::google(term.c_str(), true);
			write_cache(content, filename);
		}

		return query::retrieve_localfile(filename.c_str());
	}

	const char *algorithm_gw::get_url_and_save_cache(std::string& url, std::string& filename) {
		const char *content = NULL;
		if (!sys_file::exist(filename.c_str()) || application_settings::instance().override_cache()) {
			content = query::retrieve_webpage(url.c_str());
			write_cache(content, filename);
		}
		return query::retrieve_localfile(filename.c_str());
	}

	std::string algorithm_gw::create_wiki_en_cache_filename(std::string name)
	{
		return cache_path_ +  sys_file::SEPARATOR + "wikipedia_en_" + name + ".html";
	}

	std::string algorithm_gw::create_wiki_zh_cache_filename(std::string name)
	{
		return cache_path_ +  sys_file::SEPARATOR + "wikipedia_zh_" + name + ".html";
	}
}
