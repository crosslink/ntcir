/*
 * corpus_target.h
 *
 *  Created on: Dec 31, 2009
 *      Author: monfee
 */

#ifndef CORPUS_TARGET_H_
#define CORPUS_TARGET_H_

#include "corpus.h"

#include "pattern_singleton.h"

namespace NTCIR {

	class corpus_target: public NTCIR::corpus, public pattern_singleton<corpus_target>
	{
	public:
		corpus_target();
		virtual ~corpus_target();
	};

}

#endif /* CORPUS_TARGET_H_ */
