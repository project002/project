/*
 * CPacketCollector.h
 *
 *  Created on: Dec 17, 2013
 *      Author: root
 */

#ifndef CPACKETCOLLECTOR_H_
#define CPACKETCOLLECTOR_H_
#include "BasicIncludes.h"
#include "CPacket.h"
#include "Protocols.h"
class CPacketCollector
{
public:
	CPacketCollector(const int Socket);
	virtual ~CPacketCollector();
	// function should be called with a new thread
	void ReceivePackets();
private:
	list<CPacket * > mPackets;
	const int mSocket;
};

#endif /* CPACKETCOLLECTOR_H_ */
