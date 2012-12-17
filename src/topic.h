/*
 * topic.h
 *
 *  Created on: Dec 27, 2009
 *      Author: monfee
 */

#include <string>
#include <stdio.h>

#include <utility>
#include <vector>

#include "key_term.h"

namespace NTCIR {
	class algorithm;

	class topic {

	private:
		std::string id_;
		std::string title_;

		std::string	question_en_;
		std::string question_zh_;

		FILE *file_;
		int	number_to_list_;
		const char **docids_;
	    long long hits_;
	    std::string topic_cache_path_;

	    key_term_array_type	key_terms_;
		algorithm	*algorithm_;

		std::string src_lang_;
		std::string dest_lang_;

	public:
	    topic(std::string & id, std::string & title);
	    virtual ~topic();
	    const std::string & get_question_en() const
	    {
	        return question_en_;
	    }

	    const std::string & get_question_zh() const
	    {
	        return question_zh_;
	    }

	    void set_question_en(std::string question_en_)
	    {
	        this->question_en_ = question_en_;
	    }

	    void set_question_zh(std::string question_zh_)
	    {
	        this->question_zh_ = question_zh_;
	    }

	    std::string get_id() const
	    {
	        return id_;
	    }

	    std::string get_title() const
	    {
	        return title_;
	    }

	    void retrieve_en(const char *run_id);
	    void retrieve_zh(const char *run_id);
	    void retrieve(const char *run_id);

	    int get_number_to_list() const
	    {
	        return number_to_list_;
	    }

	    void set_number_to_list(int number_to_list)
	    {
	        this->number_to_list_ = number_to_list;
	    }

	    void add_key_term(std::string, float score);

	private:
	    void search(const char *terms, const char *run_id);
	    void create_topic_cache_path();
	    void set_algorithm();
	};

}
