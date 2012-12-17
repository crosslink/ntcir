/*
 * corpus_target.cpp
 *
 *  Created on: Dec 31, 2009
 *      Author: monfee
 */

#include "corpus_target.h"
#include "application_settings.h"

#include <string>

using namespace std;

namespace NTCIR {

	corpus_target::corpus_target()
	{
		string target_index = application_settings::instance().attributeof("target_index");
		string target_doclist = application_settings::instance().attributeof("target_doclist");

		this->doclist_filename_ = application_settings::instance().attributeof(target_doclist + "_doclist");
		this->index_filename_ = application_settings::instance().attributeof(target_index + "_index");

		//home(application_settings::instance().attributeof("target_home") + sys_file::SEPARATOR);
		bool segmentation =  application_settings::instance().attributeof("ant_segmentation") != "false";

		setup(this->doclist_filename_.c_str(), this->index_filename_.c_str(), segmentation);
	}

	corpus_target::~corpus_target()
	{

	}

}
