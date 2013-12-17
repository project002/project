/*
 * Packet.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: root
 */

#include "CPacket.h"

CPacket::CPacket(char * buffer,ssize_t & bufferSize):mSourceMacAddress(NULL),mDestinationMacAddress(NULL),mBuffer(NULL)
{
	try
	{
		mBuffer= new CBuffer(buffer,bufferSize);
	}
	catch(CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}

CPacket::~CPacket()
{
	if (mBuffer!=NULL)
	{
		delete mBuffer;
		mBuffer=NULL;
	}
	if (mDestinationMacAddress!=NULL)
	{
		delete mDestinationMacAddress;
		mDestinationMacAddress=NULL;
	}
	if (mSourceMacAddress!=NULL)
	{
		delete mSourceMacAddress;
		mSourceMacAddress=NULL;
	}
}

