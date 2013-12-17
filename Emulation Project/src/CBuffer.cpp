/*
 * CBuffer.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: root
 */

#include "CBuffer.h"

CBuffer::CBuffer():mBuffer(NULL)
{
	mSize=0;
}
CBuffer::CBuffer(char * Buffer,ssize_t & bufferSize):mBuffer(NULL),mSize(0)
{
	mBuffer = new char[bufferSize];
	memcpy(mBuffer,Buffer,bufferSize);
	mSize=bufferSize;
}
CBuffer::~CBuffer()
{
	if (mBuffer!=NULL)
	{
		delete mBuffer;
		mBuffer=NULL;
	}
}

