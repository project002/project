/*
 * CDHCPPacketHandler.h
 *
 *  Created on: Dec 24, 2013
 *      Author: root
 */

#ifndef CDHCPPACKETHANDLER_H_
#define CDHCPPACKETHANDLER_H_

#include "BasicIncludes.h"

class CDHCPPacketHandler
{
public:
	static char* mInterfaceName;
	static string mIPMaskAddress;

	static void startDHCPhandshake(Packet* sniff_packet, void* user);
	/**
	 * get the XID of the current DHCP request
	 * also filters if the request is actually a DISCOVER type
	 * @return the XID as an word
	 */
	static word getXID(Packet* sniff_packet);
};

#endif /* CDHCPPACKETHANDLER_H_ */
