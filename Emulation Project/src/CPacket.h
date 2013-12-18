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
#include "CIPv4Address.h"
#include "CBuffer.h"
/**
 * Ethernet layer information wrapper
 */
class CPacket
{
public:
	CPacket(char * buffer,ssize_t &bufferSize);
	virtual ~CPacket();
	virtual void PrintLayerHead();
	virtual void PrintLayerTail();
	virtual void Print(){};
	unsigned int GetHeadSize()const {return ETH_HLEN;}
	unsigned int GetTailSize()const {return ETH_FCS_LEN;}
protected:
	CMacAddress * mSourceMacAddress;
	CMacAddress * mDestinationMacAddress;
	CBuffer * mBuffer;
	unsigned int mFrameSequenceCheck;
	unsigned int mEthernetType;
private:
	void PrintEthernetType();
};

#endif /* PACKET_H_ */
