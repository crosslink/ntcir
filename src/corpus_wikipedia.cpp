/*
 * corpus_wikipedia.cpp
 *
 *  Created on: Sep 16, 2009
 *      Author: monfee
 */

#include "corpus_wikipedia.h"
#include "application_settings.h"
#include "sys_file.h"

using namespace NTCIR;
using namespace std;

corpus_wikipedia::corpus_wikipedia()
{
	init();
}

corpus_wikipedia::~corpus_wikipedia()
{
}

void corpus_wikipedia::init()
{
	// setup wikipedia corpus environment
	this->doclist_filename_ = application_settings::instance().attributeof("wiki_doclist");
	this->index_filename_ = application_settings::instance().attributeof("wiki_index");
	home(application_settings::instance().attributeof("wikipedia_home") + sys_file::SEPARATOR);
	setup(this->doclist_filename_.c_str(), this->index_filename_.c_str());
}

std::string corpus_wikipedia::docid2pathfile(std::string docid)
{
	return home_ + docid;
}

void corpus_wikipedia::load_wikititles(std::string filename)
{
//	if (!config_)
//		return;
//
//	string filename(application_settings::instance().attributeof("titles_file"));
//	if (filename.length() <= 0)
//		return;

//	ifstream myfile (filename.c_str());
//
//	if (myfile.is_open()) {
//		while (! myfile.eof()) {
//			string line;
//			getline (myfile, line);
//			if (line.length() == 0 /*|| line.find("Portal:") != string::npos*/)
//				continue;
//
//			string::size_type pos = line.find_first_of(':');
//			if (pos != string::npos) {
//				unsigned long doc_id = atol(line.c_str() + pos + 1);
//				string wiki_title(line.begin(), line.begin() + pos - 1);
//				std::pair<std::string, std::string> title_pair = wikipedia::process_title(wiki_title);
//
//				page_map_t::iterator iter = names_map_.find(title_pair.first);
//				if (iter == names_map_.end() || iter->second == NULL) {
//					//wiki_entry_array wea;
//					ANT_link_term *term = new ANT_link_term;
//					term->term = strdup(title_pair.first.c_str());
//					term->total_occurences = 0;
//					term->document_frequency = 9999;
//					term->collection_frequency = 9999;
//					string& name = title_pair.first;
//
//					if (iter != names_map_.end() && iter->second == NULL)
//						iter->second = term;
//					else
//						iter = names_map_.insert(make_pair(name, term)).first;
//
//					string::size_type pos = name.find(' ');
//					if (pos != string::npos) {
//						string first_term(name, 0, pos);
//						page_map_t::iterator iner_iter = names_map_.find(first_term);
//						if (iner_iter == names_map_.end())
//							names_map_.insert(make_pair(first_term, (ANT_link_term *)NULL));
////						else
////							cerr << "found a match (" << iner_iter->first << ")" << endl;
//					}
//					//result.second.push_back(a_entry);
//				}
//				iter->second->postings.push_back(a_entry);
//			}
//			else {
//				cerr << "Error in line : " << line << endl;
//			}
//
//		  //cout << line << endl;
//		}
//		myfile.close();
//	}
}
