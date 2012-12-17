/*
 * application_settings.cpp
 *
 *  Created on: Dec 30, 2009
 *      Author: monfee
 */

#include "application_settings.h"
#include "sys_file.h"
#include "config.h"
#include "algorithm_gw.h"

#include <stdexcept>

application_settings::application_settings()
{
	if (sys_file::exist("application.conf"))
		config_ = new config("application.conf");
	else
		throw std::runtime_error("can't find application.conf file.");

	cache_path_ = "cache";
	if (!sys_file::exist(cache_path_.c_str()))
		sys_file::create_directory(cache_path_.c_str());

	override_cache_ = attributeof("override_cache") == "true";
}

const std::string& application_settings::get_cache_path() const
{
    return cache_path_;
}

application_settings::~application_settings()
{
	if (config_)
		delete config_;
}

std::string application_settings::attributeof(std::string property)
{
	return config_->get_value(property);
}
