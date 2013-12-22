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
	CBuffer(const char * Buffer,int bufferSize);
	unsigned int GetSize()const{return mSize;}
	char * GetBuffer()const{return mBuffer;}
	void operator+=(const CBuffer & other);
	virtual ~CBuffer();

	void SetSize(unsigned int size)
	{
		mSize = size;
	}

private:
	char * mBuffer;
	unsigned int mSize;
};

#endif /* CBUFFER_H_ */
