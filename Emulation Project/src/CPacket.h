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
	CPacket(CMacAddress * SourceMacAddress , CMacAddress * DestinationMacAddress,uint16_t EthernetType);
	CPacket(char * buffer,ssize_t &bufferSize);
	virtual ~CPacket();
	virtual void PrintLayerHead();
	virtual void PrintLayerTail();
	virtual void Print(){};
	unsigned int GetHeadSize()const {return ETH_HLEN;}
	unsigned int GetTailSize()const {return ETH_FCS_LEN;}

	CBuffer* getBuffer() const
	{
		return mBuffer;
	}

	void setBuffer(CBuffer* buffer)
	{
		mBuffer = buffer;
	}

	CMacAddress* getDestinationMacAddress() const
	{
		return mDestinationMacAddress;
	}

	void setDestinationMacAddress(CMacAddress* destinationMacAddress)
	{
		mDestinationMacAddress = destinationMacAddress;
	}

	uint16_t getEthernetType() const
	{
		return mEthernetType;
	}

	void setEthernetType(uint16_t ethernetType)
	{
		mEthernetType = ethernetType;
	}

	unsigned int getFrameSequenceCheck() const
	{
		return mFrameSequenceCheck;
	}

	void setFrameSequenceCheck(unsigned int frameSequenceCheck)
	{
		mFrameSequenceCheck = frameSequenceCheck;
	}

	CMacAddress* getSourceMacAddress() const
	{
		return mSourceMacAddress;
	}

	void setSourceMacAddress(CMacAddress* sourceMacAddress)
	{
		mSourceMacAddress = sourceMacAddress;
	}

protected:
	CMacAddress * mSourceMacAddress;
	CMacAddress * mDestinationMacAddress;
	CBuffer * mBuffer;
	unsigned int mFrameSequenceCheck;
	uint16_t mEthernetType;
	virtual void BuildBuffer();
	virtual void BuildBufferHeader();
	virtual void BuildBufferTail();
private:
	void PrintEthernetType();
};

#endif /* PACKET_H_ */
