/*
 * chinese_segmentation.h
 *
 *  Created on: Mar 2, 2010
 *      Author: monfee
 */

#ifndef CHINESE_SEGMENTATION_H_
#define CHINESE_SEGMENTATION_H_

#include <string>

namespace NTCIR {

	class chinese_segmentation
	{
	public:
		chinese_segmentation();
		virtual ~chinese_segmentation();

		static std::string segment_bigram(const std::string& text);
		static std::string segment_ngmi(const std::string& text);
		static std::string segment_single(const std::string& text);
	};

}

#endif /* CHINESE_SEGMENTATION_H_ */
