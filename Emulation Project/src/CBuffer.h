/*
 * CBuffer.h
 *
 *  Created on: Dec 17, 2013
 *      Author: root
 */

#ifndef CBUFFER_H_
#define CBUFFER_H_
#include "BasicIncludes.h"
class CBuffer
{
public:
	CBuffer();
	CBuffer(char * Buffer,ssize_t & bufferSize);
	virtual ~CBuffer();
private:
	char * mBuffer;
	unsigned int mSize;
};

#endif /* CBUFFER_H_ */
