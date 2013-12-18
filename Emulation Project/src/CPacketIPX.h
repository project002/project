/*
 * CPacketIPX.h
 *
 *  Created on: Dec 18, 2013
 *      Author: root
 */

#ifndef CPACKETIPX_H_
#define CPACKETIPX_H_

#include "CPacket.h"

class CPacketIPX: public CPacket
{
public:
	CPacketIPX(char * buffer,ssize_t &bufferSize);
	virtual ~CPacketIPX();
	virtual void Print();
};

#endif /* CPACKETIPX_H_ */
