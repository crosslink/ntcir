/*
 * application_settings.h
 *
 *  Created on: Dec 30, 2009
 *      Author: monfee
 */

#ifndef APPLICATION_SETTINGS_H_
#define APPLICATION_SETTINGS_H_

#include <string>

#include "pattern_singleton.h"

class config;

class application_settings : public pattern_singleton<application_settings>
{
private:
	config		*config_;
	std::string cache_path_;
	bool is_ct_; // flag for whether retrieve traditional Chinese version of wiki
	bool override_cache_;

public:
	application_settings();
	virtual ~application_settings();

	std::string attributeof(std::string property);
    const std::string& get_cache_path() const;

    bool is_ct() const
    {
        return is_ct_;
    }

    void is_ct(bool is_ct_)
    {
        this->is_ct_ = is_ct_;
    }

    bool override_cache()
    {
    	return override_cache_;
    }

};

#endif /* APPLICATION_SETTINGS_H_ */
