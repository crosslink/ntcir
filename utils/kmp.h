/*
 * kmp.h
 *
 *  Created on: Feb 20, 2012
 *      Author: monfee
 */

#ifndef KMP_H_
#define KMP_H_

class kmp {
public:
	kmp(char *pattern);
	virtual ~kmp();

	int search(char *dataToSearch);
	int search(char * dataToSearch, int start);
	int search(char * dataToSearch, int start, int length);

private:
	char *pattern_;
	int next_[];
	int pattern_len_;
};

#endif /* KMP_H_ */
