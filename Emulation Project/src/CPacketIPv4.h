/*
 * CPacketIPv4.h
 *
 *  Created on: Dec 18, 2013
 *      Author: root
 */

#ifndef CPACKETIPV4_H_
#define CPACKETIPV4_H_

#include "CPacket.h"

class CPacketIPv4: public CPacket
{
public:
	CPacketIPv4(char * buffer,ssize_t &bufferSize);
	virtual ~CPacketIPv4();
	virtual void Print();
protected:
	uint8_t mVersion :4;
	uint8_t mHeaderLength :4;

	uint8_t mServiceType;
	uint16_t mTotalLength;
	uint16_t mIdentification;
	uint16_t mFlags : 3;
	uint16_t mFragmentationOffset :5;
	uint8_t mTimeToLive;
	uint8_t mProtocol;
	uint16_t mHeaderChecksum;
	CIPv4Address * mSourceIPAddress;
	CIPv4Address * mDestinationIPAddress;
	uint64_t mOptions;

private:
	virtual void PrintLayerHead();
	virtual void PrintLayerTail();
};

#endif /* CPACKETIPV4_H_ */
