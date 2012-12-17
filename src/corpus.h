/*
 * corpus.h
 *
 *  Created on: Sep 16, 2009
 *      Author: monfee
 */

#ifndef CORPUS_H_
#define CORPUS_H_

#include <map>
#include <string>

class search_engine_ant;

namespace NTCIR {

	class corpus
	{
	public:
		typedef std::map<std::string, std::string> 	doc_map;

	protected:
		//doc_map									doc_map_;
		bool									load_;
		std::string								home_;
		std::string								base_;

		search_engine_ant						*ant_;
//		char 				**docids_;
//		long				hits;

		std::string								index_filename_;
		std::string								doclist_filename_;

	public:
		corpus();
		corpus(const char *doclist);
		virtual ~corpus();

		void home(std::string path) { home_ = path; }
		void base(std::string path) { base_ = path; }
		void load();


	    std::string get_doclist_filename() const
	    {
	        return doclist_filename_;
	    }

	    std::string get_index_filename() const
	    {
	        return index_filename_;
	    }

		void setup(const char *doclist_filename, const char *index_filename, bool segmentation = true);
		const char **search(const char *query, long long *hits/*const char **docids*/);

		virtual std::string ext() { return ""; }

		//static corpus& instance();

	private:
		void init();
	};

//	inline corpus& corpus::instance() {
//		static corpus inst;
//		return inst;
//	}
}

#endif /* CORPUS_H_ */
