/*
 * CPacketIPv6.h
 *
 *  Created on: Dec 18, 2013
 *      Author: root
 */

#ifndef CPACKETIPV6_H_
#define CPACKETIPV6_H_

#include "CPacket.h"
#include "CIPv6Addres.h"
class CPacketIPv6: public CPacket
{
public:
	CPacketIPv6(char * buffer,ssize_t &bufferSize);
	virtual ~CPacketIPv6();
	virtual void Print();
protected:
	uint8_t mVersion :6;
	uint8_t mTrafficClass;
	uint32_t mFlowLabel:20;
	uint16_t mPayloadLength;
	uint8_t mNextHeader;
	uint8_t mHopLimit;
	CIPv6Address * mSourceIPAddress;
	CIPv6Address * mDestinationIPAddress;
private:
	virtual void PrintLayerHead();
	virtual void PrintLayerTail();
};

#endif /* CPACKETIPV6_H_ */
