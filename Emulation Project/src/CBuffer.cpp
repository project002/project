/*
 * CBuffer.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: root
 */

#include "CBuffer.h"

CBuffer::CBuffer():mBuffer(NULL)
{
	mBuffer=new char[0];
	mSize=0;
}
CBuffer::CBuffer(char * Buffer,ssize_t & bufferSize):mBuffer(NULL),mSize(0)
{
	mBuffer = new char[bufferSize];
	memcpy(mBuffer,Buffer,bufferSize);
	mSize=bufferSize;
}
void CBuffer::operator+=(const CBuffer & other)
{
	try
	{
		char * newBuffer = new char[mSize + other.mSize];
		memcpy(newBuffer,mBuffer,mSize);
		memcpy(newBuffer+mSize,other.mBuffer,other.mSize);
		delete mBuffer;
		mBuffer=newBuffer;
	}
	catch(CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}
CBuffer::~CBuffer()
{
	if (mBuffer!=NULL)
	{
		delete mBuffer;
		mBuffer=NULL;
	}
}

