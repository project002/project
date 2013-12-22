/*
 * CPacketCollector.h
 *
 *  Created on: Dec 17, 2013
 *      Author: root
 */

#ifndef CPACKETCOLLECTOR_H_
#define CPACKETCOLLECTOR_H_
#include "BasicIncludes.h"
#include "PacketTypes.h"
class CPacketCollector
{
public:
	CPacketCollector(const int Socket);
	virtual ~CPacketCollector();
	// function should be called with a new thread
	void ReceivePackets();
	void SendPacket(char * buffer, ssize_t recvSize);
private:
	list<CPacket * > mReceivedPackets;
	list<CPacket * > mTransmittedPackets;
	const int mSocket;
private:
	CPacket * CreatePacket(char * buffer, ssize_t recvSize);
	CPacket * CreateIPv4Packet(char * buffer, ssize_t recvSize);
	CPacket * CreateIPv6Packet(char * buffer, ssize_t recvSize);
	CPacket * CreateIPXPacket(char * buffer, ssize_t recvSize);
	CPacket * CreateARPPacket(char * buffer, ssize_t recvSize);
	void PrintPacket(char * buffer, ssize_t recvSize);
};

#endif /* CPACKETCOLLECTOR_H_ */
