/*
 * Color.h
 *
 *  Created on: Dec 18, 2013
 *      Author: root
 */

#ifndef COLOR_H_
#define COLOR_H_

#define COLOR_NORMAL "\x1B[0m"
#define COLOR_RED "\x1B[31m"
#define COLOR_GREEN "\x1B[32m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_BLUE "\x1B[34m"
#define COLOR_WHITE "\x1B[37m"

static inline void LogColor(const char * text, const char * color)
{
	printf("%s%s",color,text);
	printf("%s",COLOR_NORMAL);
}

#endif /* COLOR_H_ */
