/*
 * algorithm_gw2.h
 *
 *  Created on: Feb 16, 2010
 *      Author: monfee
 */

#ifndef ALGORITHM_GW2_H_
#define ALGORITHM_GW2_H_

#include "algorithm_gw.h"
#include "key_term.h"
#include <string>
#include <vector>

namespace NTCIR {

	class algorithm_gw2: public NTCIR::algorithm_gw
	{
	public:
		static const char *EMPTY_WIKI_ZH_TERM;

		enum { GW2_TEST1 = 0, GW2_TEST2, GW2_TEST3, GW2_TEST4, WIKI_AND_CLUE_V2, WIKI_AND_CLUE, WIKI_ONLY };

	private:
		key_term_array_type 		query_terms_;
		int 						gw2_function_;
		std::vector<key_term> 		google_query_terms_;

	public:
		algorithm_gw2();
		virtual ~algorithm_gw2();

		virtual void test1();
		virtual void test2();
		virtual void test3();
		virtual void test4();
		virtual void wiki_and_clue2();
		virtual void wiki_only();
		virtual void wiki_and_clue();

	protected:
		void process_tip_term_mt(std::string& mt_tran, std::string& tip_term_tran);
		void wiki_and_clue2(std::string& question);
		void wiki();
		void wiki2();
		void get_cluetext_translation(std::vector<std::string>& term_array, const char *content);
		void gw(std::string& question, key_term_array_type& key_terms);
		virtual void start(std::string& question, key_term_array_type& key_terms);

		//void print_terms(std::string& term, std::string tm, )
		std::string create_text_for_translation(std::string& term, const char *content);
	};

}

#endif /* ALGORITHM_GW2_H_ */
