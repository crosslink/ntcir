/*
 * topic_set.h
 *
 *  Created on: Dec 27, 2009
 *      Author: monfee
 */

#include <vector>

#include "topic.h"

namespace NTCIR {

	class topic_set {
	private:
		std::vector<topic>	topic_array_;

	public:
		topic_set(char *topic_file);
		virtual ~topic_set();

		std::vector<topic>& topic_array() { return topic_array_; }

	private:
		void parse(char *topic_file);
	};

}
