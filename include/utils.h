/*
 * utils.h
 *
 *      Author: Andrew Trotman
 */

#ifndef UTILS_H_
#define UTILS_H_

inline void cpoint_to_utf8bytes(char *buf, long cp)
{
	if (cp < 0x80) {				// 1-byte (ASCII) character
		buf[0] = cp;
		buf[1] = '\0';
	}
	else if (cp < 0x800/*(*here & 0xE0) == 0xC0*/)	{ // 2-byte sequence
		//return ((*here & 0x1F) << 6) | (*(here + 1) & 0x3F);
		buf[0] = 0xC0 | (cp >> 6);
		buf[1] = 0xC0 | (cp & 0x3F);
		buf[2] = '\0';
	}
	else if (cp < 0x10000/*(*here & 0xF0) == 0xE0*/)	 { // 3-byte sequence
		buf[0] = 0xE0 | (cp >> 12);;
		buf[1] = 0x80 | ((cp & 0xFC0) >> 6);
		buf[2] = 0x80 | (cp & 0x3F);
		//return ((*here & 0x0F) << 12) | ((*(here + 1) & 0x3F) << 6) | (*(here + 2) & 0x3F);
		buf[3] = '\0';
	}
	else if (cp < 0x11000/*(*here & 0xF8) == 0xF0*/)	{// 4-byte sequence
		buf[0] = 0xF0 | (cp >> 18);;
		buf[1] = 0x80 | ((cp & 0x3F000) >> 12);
		buf[2] = 0x80 | ((cp & 0xFC0) >> 6);
		buf[3] = 0x80 | (cp & 0x3F);
		//return ((*here & 0x03) << 18) | ((*(here + 1) & 0x3F) << 12) | ((*(here + 2) & 0x3F) << 6) | (*(here + 1) & 0x3F);
		buf[4] = '\0';
	}
	else
		buf[0] = '\0';
}

#endif /* UTILS_H_ */
