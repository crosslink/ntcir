/*
 * algorithm.h
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee
 */

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <string>

#include "key_term.h"

class config;

namespace NTCIR {

	/*
	 *
	 */
	class algorithm {
	public:
		enum run_mode { WITH_CMDLINE_OPTIONS, SIMPLE };
		enum { SEGMENTATION_NONE = 0, SEGMENTATION_BIGRAM = 2, SEGMENTATION_NGMI = 4, SEGMENTATION_SINGLE = 8 };

	public:
		static const char *CHINESE_QUERY_TERMS_FILE;
		static const char *CHINESE_QUERY_ANALYSIS_FILE;

	protected:
		run_mode 				mode_;
		config					*config_;
		std::string 			terms_;  // query terms
		std::vector<key_term>	mt_terms_;

		std::string 			machine_translation_; // MT result, either on the whole question,
		std::string 			machine_translation_pruned_; //or pruned question
		std::string 			cluetext_translation_;
		std::string				wiki_translation_;
		std::string				expansion_terms_;

		int						query_segmentation_method_;
		//bool					need_google_translate_;
		bool					use_pruned_question_;
		bool					use_entire_question_;
		bool					prune_after_translation_;
		bool					prune_question_;
		bool					use_cluetext_translation_;
		bool					use_pruned_translation_;

		std::string 			src_lang_;
		std::string 			dest_lang_;
		std::string 			langpair_;

		bool 					set_locale_;
		std::string 			locale_;
		std::string 			cache_path_;

	public:
		algorithm();
		virtual ~algorithm();

		//void start() = 0;
		//virtual void start(std::string& question) = 0;
		virtual void start(std::string& question, key_term_array_type& key_terms);
		const std::string& get_terms();

	    void set_dest_lang(std::string dest_lang)
	    {
	        this->dest_lang_ = dest_lang;
	    }

	    void set_src_lang(std::string src_lang)
	    {
	        this->src_lang_ = src_lang;
	    }

	    void set_cache_path(std::string cache_path)
	    {
	    	this->cache_path_ = cache_path;
	    }

	protected:
		void create_unique_query_terms(std::string& terms);

	    std::string create_google_translate_cache_filename(std::string name);
	    const char *get_google_translate_and_save_cache(std::string& term, std::string& filename);
	    const char *get_google_translate_and_save_cache(std::string& term);

	    void process_filename(std::string& filename);
	    void write_cache(const char *content, std::string filename);

		std::string create_text_for_translation(std::vector<key_term>& term_array);
		std::string get_mt(key_term_array_type& key_terms); // get translation using MT for example, google online translation service
		void save_query_terms(key_term_array_type& key_terms);
		void get_mt_question(std::string& question);
		void get_mt_question(std::string& question, std::string& where_to_save);

		std::string remove_questionstopwords(std::string& question, std::string lang);
	};

}

#endif /* ALGORITHM_H_ */
