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
};

#endif /* CPACKETIPV4_H_ */
