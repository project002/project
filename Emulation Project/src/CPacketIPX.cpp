/*
 * CPacketIPX.cpp
 *
 *  Created on: Dec 18, 2013
 *      Author: root
 */

#include "CPacketIPX.h"

CPacketIPX::CPacketIPX(char * buffer,ssize_t &bufferSize):CPacket(buffer,bufferSize)
{
	// TODO Auto-generated constructor stub

}

CPacketIPX::~CPacketIPX()
{
	// TODO Auto-generated destructor stub
}

void CPacketIPX::Print()
{
	CPacket::PrintLayerHead();
	//TODO : Print IPX information Here
	CPacket::PrintLayerTail();
}
