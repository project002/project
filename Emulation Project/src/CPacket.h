/*
 * Packet.h
 *
 *  Created on: Dec 17, 2013
 *      Author: root
 */

#ifndef PACKET_H_
#define PACKET_H_
#include "BasicIncludes.h"
#include "CMacAddress.h"
#include "CBuffer.h"
class CPacket
{
public:
	CPacket(char * buffer,ssize_t &bufferSize);
	virtual ~CPacket();
private:
	CMacAddress * mSourceMacAddress;
	CMacAddress * mDestinationMacAddress;
	CBuffer * mBuffer;
};

#endif /* PACKET_H_ */
