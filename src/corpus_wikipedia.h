/*
 * corpus.h
 *
 *  Created on: Sep 16, 2009
 *      Author: monfee
 */

#ifndef CORPUS_WIKIPEDIA_H_
#define CORPUS_WIKIPEDIA_H_

#include <map>
#include <string>

#include "corpus.h"

namespace NTCIR {

	class corpus_wikipedia: public corpus
	{
	private:
		doc_map				titles_;

	public:
		corpus_wikipedia();
		virtual ~corpus_wikipedia();

		virtual std::string ext() { return ".xml"; }
		void load_wikititles(std::string filename);

		static corpus_wikipedia& instance();

		virtual std::string docid2pathfile(std::string id);

	private:
		void init();
	};

	inline corpus_wikipedia& corpus_wikipedia::instance() {
		static corpus_wikipedia inst;
		return inst;
	}
}

#endif /* CORPUS_WIKIPEDIA_H_ */
