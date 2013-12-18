/*
 * CPacketIPv6.cpp
 *
 *  Created on: Dec 18, 2013
 *      Author: root
 */

#include "CPacketIPv6.h"

CPacketIPv6::CPacketIPv6(char * buffer,ssize_t &bufferSize):CPacket(buffer,bufferSize)
{
	// TODO Auto-generated constructor stub

}

CPacketIPv6::~CPacketIPv6()
{
	// TODO Auto-generated destructor stub
}

void CPacketIPv6::Print()
{
	CPacket::PrintLayerHead();
	//TODO : Print IPv6 information Here
	CPacket::PrintLayerTail();
}
