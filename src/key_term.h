/*
 * key_term.h
 *
 *  Created on: Feb 18, 2010
 *      Author: monfee
 */

#ifndef KEY_TERM_H_
#define KEY_TERM_H_

#include <vector>
#include <string>

namespace NTCIR {

	class key_term
	{
	public:
		typedef std::vector<key_term> 	key_term_array_type;

	public:
		int								id;
		int								tag_id;
		float 							score;
		std::string 					term;
		std::string						tip_term;
		std::string						tip_term_tran;
		char							lang[6];
		std::vector<std::string>		trans;
		std::string						mt_tran;
		std::string						desc;

//	public:
//		key_term();
//		virtual ~key_term();
	public:
		static void load_key_terms(std::string& filename, key_term_array_type& key_terms);
	};

	typedef key_term::key_term_array_type 	key_term_array_type;

}

#endif /* KEY_TERM_H_ */
