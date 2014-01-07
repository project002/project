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
	CPacketCollector(unsigned int bufferSize);
	virtual ~CPacketCollector();
	void PushBack(Crafter::Packet * pkt);
	/**
	 * @return if empty NULL is returned
	 */
	Crafter::Packet * PopFront();
	void DropRandomPacket();
private:
	void PrintPacket();
	list<Crafter::Packet * > mPackets;
	unsigned int mBufferSize;
	static map<string,unsigned long long > mPacketsStatistics;
};

#endif /* CPACKETCOLLECTOR_H_ */
