/*
 * types.h
 *
 *  Created on: Feb 16, 2010
 *      Author: monfee
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <utility>
#include <vector>

#include "key_term.h"

namespace NTCIR {
		typedef std::pair<std::string, float>	key_term_type;
		typedef std::vector<key_term_type >  key_term_array_type;
}

#endif /* TYPES_H_ */
