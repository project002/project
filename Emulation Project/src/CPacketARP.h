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

class CPacketARP: public CPacket
{
public:
	CPacketARP(char * buffer,ssize_t &bufferSize);
	virtual ~CPacketARP();
	virtual void Print();
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
private:
	void PrintLayerHead();
	void PrintLayerTail();
	void PrintProtocolType();
	void PrintHardwareType();
	void PrintOpCode();
};

#endif /* CPACKETARP_H_ */

