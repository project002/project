/*
 * CPacketCollector.h
 *
 *  Created on: Dec 17, 2013
 *      Author: root
 */

#ifndef CPACKETCOLLECTOR_H_
#define CPACKETCOLLECTOR_H_
#include "BasicIncludes.h"
class CPacketCollector
{
public:
	CPacketCollector(const int Socket);
	virtual ~CPacketCollector();
	// function should be called with a new thread
	void ReceivePackets();
	void SendPacket(char * buffer, ssize_t recvSize);
private:
	const int mSocket;
private:

	void PrintPacket(char * buffer, ssize_t recvSize);
};

#endif /* CPACKETCOLLECTOR_H_ */
