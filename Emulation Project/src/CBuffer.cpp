/*
 * CBuffer.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: root
 */

#include "CBuffer.h"

CBuffer::CBuffer() :
		mBuffer(NULL)
{
	try
	{
		mBuffer = new char[0];
		mSize = 0;
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}
CBuffer::CBuffer(char * Buffer, ssize_t & bufferSize) :
		mBuffer(NULL), mSize(0)
{
	try
	{
		mBuffer = new char[bufferSize];
		memcpy(mBuffer, Buffer, bufferSize);
		mSize = bufferSize;
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}
CBuffer::CBuffer(const char * Buffer,int bufferSize):mBuffer(NULL),mSize(bufferSize)
{
	try
	{
		mBuffer = new char[bufferSize];
		memcpy(mBuffer, Buffer, bufferSize);
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}
void CBuffer::operator+=(const CBuffer & other)
{
	try
	{
		char * newBuffer = new char[mSize + other.mSize];
		memcpy(newBuffer, mBuffer, mSize);
		memcpy(newBuffer + mSize, other.mBuffer, other.mSize);
		delete mBuffer;
		mBuffer = newBuffer;
		mSize+=other.mSize;
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}
CBuffer::~CBuffer()
{
	try
	{
		if (mBuffer != NULL)
		{
			delete mBuffer;
			mBuffer = NULL;
		}
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

