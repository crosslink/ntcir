/*
 * query.cpp
 *
 *  Created on: Dec 26, 2009
 *      Author: monfee
 */

#include "query.h"
#include "webpage_retriever.h"
#include "ant_link_parts.h"
#include "application_settings.h"
#include "language.h"
#include "string_utils.h"
#include "stop_word.h"

#include <string>
#include <iostream>

using namespace std;

namespace NTCIR {

	const char *query::GOOGLE_SEARCH = "http://www.google.com.au/search?hl=en&ie=utf-8&oe=utf-8&safe=off&q=";

	query::query()
	{

	}

	query::~query()
	{
	}

	void query::generate_query(const char *query)
	{

	}

	void query::generate_en2zh_query(const char *query)
	{
		/* using a two step query generation using google and wikipedia */

	}

	char *query::google(const char *query, bool set_lr)
	{
		google_site(query, "", set_lr);
	}

	char *query::google_site(const char *query, const char *site, bool set_lr)
	{
		string query_string(GOOGLE_SEARCH);

		//static ANT_stop_word stop_word;
		const char *start = NULL, *end = NULL;
		start = end = query;
		while (*start != '\0') {
			while (isspace(*start))
				++start;

			if (*start == '\0')
				break;

			end = start;
			while (*end != '\0' && !isspace(*end))
				++end;

			std::string term(start, end);

//			if ((set_lr && language::isstopword(term.c_str())))
//				skip = true;

			query_string.append(term);
			query_string.append("+");

			if (*end == '\0')
				break;

			start = end;
		}
//		for (first = term_list; *first != NULL; first++) {
//			//if (!stop_word.isstop(strlower(*first))) {
//			if (/*strlen(*first) == 1 ||*/ (set_lr && language::isstopword(strlower(*first))))
//				skip = true;
//
//			if (!skip) {
//				query_string.append(*first);
//				query_string.append("+");
//			}
//			else
//				skip = false;
//		}

		if (strlen(site) > 0) {
			query_string.append("site%3A");
			query_string.append(site);
		}

		if (set_lr) {
		//append return languages
			static const char* lang_underscore = "lang_";
			query_string.append("&lr=");
			string langpair = application_settings::instance().attributeof("LANGPAIR");
			langpair.insert(0, lang_underscore);
			string::size_type pos = 0;
			while ((pos = langpair.find("|", pos)) != string::npos) {
				++pos;
				langpair.insert(pos, lang_underscore);
			}
			query_string.append(langpair);
		}

		char * content = retrieve_webpage(query_string.c_str());

		//cout << content << endl;
		return content;
	}

	char *query::retrieve_webpage(const char *url)
	{
		char * content = webpage_retriever::instance().retrieve(url);
		return content;
	}

	char *query::retrieve_localfile(const char *file)
	{
		string url("FILE://");
		url.append(file);

		return webpage_retriever::instance().retrieve(url.c_str());
	}
}
