/*
 * query.h
 *
 *  Created on: Dec 28, 2009
 *      Author: monfee
 */

#ifndef QUERY_H_
#define QUERY_H_

namespace NTCIR {

	class query
	{
	public:
		static const char *GOOGLE_SEARCH;

	public:
		query();
		virtual ~query();

		void generate_query(const char *query);
		void generate_en2zh_query(const char *query);
		static char *google(const char *query, bool set_lr = false);
		static char *google_site(const char *query, const char *site, bool set_lr = false);

		static char *retrieve_webpage(const char *url);
		static char *retrieve_localfile(const char *file);
	};

}

#endif /* QUERY_H_ */
