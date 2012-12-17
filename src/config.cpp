/*
 * config.cpp
 *
 *  Created on: Aug 4, 2009
 *      Author: monfee
 */

#include "config.h"
#include <stpl/stpl_property.h>
#include <stpl/stpl_parser.h>
#include <iostream>

using namespace std;
using namespace stpl;
//using namespace ET;

config::config(const char *name) : name_(name) {
	load();

#ifdef DEBUG
	show();
#endif
}

config::~config() {

}

void config::load()
{
	// create a STPL parser for reading the property file
	properties_.read(name_);
	PropertiesFileParser property_parser(&properties_);

	property_parser.parse();
}

void config::show()
{
	cerr << "------------------- SETTINGS in " << name_ << "------------------------------------" << endl;
	PropertiesFile::entity_iterator it = properties_.iter_begin();
	for (; it != properties_.iter_end(); it++)
		if ((*it)->name()[0] != '#')
			cerr << (*it)->name() << ":" << (*it)->value() << endl;
	cerr << "-------------------------------------------------------------------------------" << endl;
}

config::string_type	config::get_value(string_type name)
{
	stpl::PropertiesFile::entity_iterator it = properties_.find(name);
	return it != properties_.iter_end() ? (*it)->value() : "";
}
