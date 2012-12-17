/*
 * algorithm_gw.h
 *
 *  Created on: Jan 3, 2010
 *      Author: monfee
 */

#ifndef ALGORITHM_GW_H_
#define ALGORITHM_GW_H_

#include "algorithm.h"
#include "language.h"

#include <map>
#include <string>
#include <vector>
#include <freq.h>

namespace NTCIR {

	class algorithm_gw : public algorithm
	{
	public:
		static const char *GOOGLE_CACHE_FILE1; // first search using all terms, if there is no wikipedia page
		static const char *GOOGLE_CACHE_FILE2; // second search using the terms extracted from titles of results in first search, limited in en.wikipedia.org only
		static const char *WIKIPEDIA_CACHE_FILE_EN; //
		static const char *WIKIPEDIA_CACHE_FILE_ZH; //
		static const char *CHINESE_TEXT_FILE;
		static const char *GOOLGE_TRANSLATION_TEXT_FILE;

		static const char *ALGORITHM_GW_CONFIG_FILE;

	protected:
		std::map<std::string, int> 	term_set_;
		std::string					wiki_url_;
		std::vector<std::string>	all_wiki_urls_;

		std::string google_cache_file1_;
		std::string google_cache_file2_;
		std::string wikipedia_cache_file_en_;
		std::string wikipedia_cache_file_zh_;
		std::string chinese_text_file_;
		std::string google_translate_text_file_;

		std::string chinese_terms_;
		std::string trans_;

		bool with_translation_;

	public:
		algorithm_gw();
		virtual ~algorithm_gw();

		//virtual void start(std::string& question);
		virtual void start(std::string& question, key_term_array_type& key_terms);

	    const std::string& get_terms() const { return terms_; }

	protected:
		void create_term_set(const char *source);

		void segment_terms(std::string& terms);

		virtual void test1(std::string& question, const char *language_pair);
		virtual void test2(std::string& question, const char *language_pair);
		virtual void test3(std::string& question, const char *language_pair);
		virtual void test4(std::string& question, const char *language_pair);
		virtual void test5(std::string& question, const char *language_pair);
		virtual void test6(std::string& question, const char *language_pair);

		virtual void gw(std::string& question);
		void get_google_translate(std::string& question, std::string& terms, const char *language_pair);

		virtual bool process_google_results(const char *content);
		bool process_google2_results(const char *content, std::string& question);

		std::string process_wikipedia_en_page(const char *content); // return zh wiki url
		std::string get_wikipedia_page_title(const char *content); // return Chinese terms

		const std::string& get_wiki_url() const { return wiki_url_; }
		std::pair<std::string, std::pair<std::string, std::string> > get_wiki_title(std::string& wiki_url);
		void get_google2_terms(std::string& terms, int threshold = 3);
		void get_chinese_freq_terms(std::string& terms, /*Freq*/language::term_freq_map_type& freq, int term_count = 0, int threshold = 5);
		void get_chinese_freq_terms(std::string& terms, Freq& freq, int threshold = 5);

		std::string cal_chinese_terms_freq(std::string& question, const char *language_pair);

		void collect_chinese(const char *content, bool forceread = false);

	    std::string create_wiki_en_cache_filename(std::string name);
	    std::string create_wiki_zh_cache_filename(std::string name);

	    const char *google_and_save_cache(std::string& url, std::string& filename);
	    const char *get_url_and_save_cache(std::string& url, std::string& filename);
	};

}

#endif /* ALGORITHM_GW_H_ */
