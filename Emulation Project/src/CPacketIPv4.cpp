/*
 * CPacketIPv4.cpp
 *
 *  Created on: Dec 18, 2013
 *      Author: root
 */

#include "CPacketIPv4.h"

CPacketIPv4::CPacketIPv4(char * buffer,ssize_t &bufferSize):CPacket(buffer,bufferSize)
{

}

CPacketIPv4::~CPacketIPv4()
{

}

void CPacketIPv4::Print()
{
	CPacket::PrintLayerHead();
	//TODO : Print IPv4 information Here
	CPacket::PrintLayerTail();
}

