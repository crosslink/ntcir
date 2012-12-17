/*
 * kmp.cpp
 *
 *  Created on: Feb 20, 2012
 *      Author: monfee
 */

#include "kmp.h"

kmp::kmp(char *pattern) {
	pattern_ = strdup(pattern);
	pattern_len_ = strlen(pattern_);
	next_ = new int[pattern_len_];
	next_[0] = -1;
	for (int len = pattern_len_-1, i = 0, j = -1; i < len; next_[++i] = ++j)
	{
		while((j >= 0) && (pattern_[i] != pattern_[j]))
		{
			j = next_[j];
		}
	}
}

kmp::~kmp() {
	free(pattern_);
	delete [] next_;
}

/**
 * Searches a range of byte from a start index
 * in a byte array for the pattern used in the construction
 * of this agent. Returns the index of the start of the match or -1
 * if the pattern is not found.
 *
 * @param dataToSearch the data to search.
 * @param start the start point in the data
 * @param length the number of byte to search.
 * @return the index of the start of the match or -1 if not found.
 */
int kmp::search(char *dataToSearch, int start, int length)
{
	int end = start + length;
	if (end > strlen(dataToSearch))
		throw ("Invalid range");

	int i = start;
	int j = 0;
	for (; (j < pattern_len_) && (i < end); i++, j++)
	{
		while ((j >= 0) && (dataToSearch[i] != pattern_[j]))
		{
			j = next_[j];
		}
	}
	return (j == pattern_len_) ? i - pattern_len_ : -1;
}

/**
 * Searches from a start index until the end of the data
 * in a byte array for the pattern used in the construction
 * of this agent. Returns the index of the start of the match or -1
 * if the pattern is not found.
 *
 * @param dataToSearch the data to search.
 * @param start the start point in the data
 * @return the index of the start of the match or -1 if not found.
 */
int kmp::search(char * dataToSearch, int start)
{
	return search(dataToSearch, start, strlen(dataToSearch) - start);
}

/**
 * Searches from the start until the end of the data
 * in a byte array for the pattern used in the construction
 * of this agent. Returns the index of the start of the match or -1
 * if the pattern is not found.
 *
 * @param dataToSearch the data to search.
 * @return the index of the start of the match or -1 if not found.
 */
int kmp::search(char * dataToSearch)
{
	return search(dataToSearch, 0);
}

