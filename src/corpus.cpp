/*
 * corpus.cpp
 *
 *  Created on: Sep 16, 2009
 *      Author: monfee
 */

#include "corpus.h"
//#include "sys_files.h"
#include "search_engine_ant.h"

#include <string>
#include <fstream>
#include <utility>
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace NTCIR;
using namespace std;

corpus::corpus()
{
	init();
}

corpus::~corpus()
{
	if (ant_)
		delete ant_;
}

void corpus::init()
{
	load_ = false;
	ant_ = NULL;
}

void corpus::load()
{
	if (!load_)
		return;

//	ifstream myfile (doclist_);
//
//	string line;
//	if (myfile.is_open()) {
//		while (! myfile.eof()) {
//		  getline (myfile, line);
//		  string name(line, 0, line.find_last_of('.'));
//		  if (line.length() > 0) {
//			  std::string doc_id = result_to_id(line.c_str());
//			  doc_map_.insert(make_pair(doc_id, name));
//		  }
//		  //cout << line << endl;
//		}
//		myfile.close();
//	}
}

void corpus::setup(const char *doclist_filename, const char *index_filename, bool segmentation)
{
	ant_ = new search_engine_ant(doclist_filename, index_filename, segmentation);
}

//long corpus::search(const char *query, /*const */char **docids)
const char **corpus::search(const char *query, long long *hits/*const char **docids*/)
{
	if (!ant_)
		return 0;

	const char **docids = ant_->search(query);
	*hits = ant_->hits();
	return docids;
}
