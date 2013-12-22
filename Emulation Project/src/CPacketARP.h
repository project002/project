/*
 * CPacketARP.h
 *
 *  Created on: Dec 18, 2013
 *      Author: root
 */

#ifndef CPACKETARP_H_
#define CPACKETARP_H_

#include "CPacket.h"

#define ARP_ETHERNET_PROTOCOL 0x1
#define ARP_IP_PROTOCOL 0x800
#define ARP_REQUEST_OP_CODE 0x1
#define ARP_REPLY_OP_CODE 0x2
#define RARP_REQUEST_OP_CODE 0x3
#define RARP_REPLY_OP_CODE 0x4

#define ARP_HLEN 28

class CPacketARP: public CPacket
{
public:
	CPacketARP(CMacAddress * MacSource,CIPv4Address *IPv4Source, CIPv4Address *IPv4Destination);//Creating Basic Arp Packet /*Ether type = arp 0x0806*/
	CPacketARP(char * buffer,ssize_t &bufferSize);
	virtual ~CPacketARP();
	virtual void Print();
	virtual void BuildBuffer();

	const CIPv4Address* getDestinationIpAddress() const
	{
		return mDestinationIPAddress;
	}

	void setDestinationIpAddress(CIPv4Address* destinationIpAddress)
	{
		mDestinationIPAddress = destinationIpAddress;
	}

	const CMacAddress* getDestinationMacAddress() const
	{
		return mDestinationMacAddress;
	}

	void setDestinationMacAddress(CMacAddress* destinationMacAddress)
	{
		mDestinationMacAddress = destinationMacAddress;
	}

	uint8_t getHardwareAddressLength() const
	{
		return mHardwareAddressLength;
	}

	void setHardwareAddressLength(uint8_t hardwareAddressLength)
	{
		mHardwareAddressLength = hardwareAddressLength;
	}

	uint16_t getHardwareType() const
	{
		return mHardwareType;
	}

	void setHardwareType(uint16_t hardwareType)
	{
		mHardwareType = hardwareType;
	}

	uint16_t getOpCode() const
	{
		return mOpCode;
	}

	void setOpCode(uint16_t opCode)
	{
		mOpCode = opCode;
	}

	uint8_t getProtocolAddressLength() const
	{
		return mProtocolAddressLength;
	}

	void setProtocolAddressLength(uint8_t protocolAddressLength)
	{
		mProtocolAddressLength = protocolAddressLength;
	}

	uint16_t getProtocolType() const
	{
		return mProtocolType;
	}

	void setProtocolType(uint16_t protocolType)
	{
		mProtocolType = protocolType;
	}

	const CIPv4Address* getSourceIpAddress() const
	{
		return mSourceIPAddress;
	}

	void setSourceIpAddress(CIPv4Address* sourceIpAddress)
	{
		mSourceIPAddress = sourceIpAddress;
	}

	const CMacAddress* getSourceMacAddress() const
	{
		return mSourceMacAddress;
	}

	void setSourceMacAddress(CMacAddress* sourceMacAddress)
	{
		mSourceMacAddress = sourceMacAddress;
	}

protected:

	uint16_t mHardwareType; // Should be Ethernet = 0x1
	uint16_t mProtocolType; // Should be IP = 0x800
	uint8_t mHardwareAddressLength; // Should be MAC length = 0x6
	uint8_t mProtocolAddressLength; // Should be IPv4 length = 0x4 since IPv6 is returned by NDP protocol and not ARP
	uint16_t mOpCode; // When received it is a Request op code = 0x1
	CMacAddress *mSourceMacAddress;
	CIPv4Address *mSourceIPAddress;
	CMacAddress *mDestinationMacAddress;
	CIPv4Address *mDestinationIPAddress;
	virtual void BuildBufferTail();
	virtual void BuildBufferHeader();
private:
	void PrintLayerHead();
	void PrintLayerTail();
	void PrintProtocolType();
	void PrintHardwareType();
	void PrintOpCode();
};

#endif /* CPACKETARP_H_ */

