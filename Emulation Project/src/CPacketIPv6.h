/*
 * CPacketIPv6.h
 *
 *  Created on: Dec 18, 2013
 *      Author: root
 */

#ifndef CPACKETIPV6_H_
#define CPACKETIPV6_H_

#include "CPacket.h"
class CPacketIPv6: public CPacket
{
public:
	CPacketIPv6(char * buffer,ssize_t &bufferSize);
	virtual ~CPacketIPv6();
	virtual void Print();
};

#endif /* CPACKETIPV6_H_ */
