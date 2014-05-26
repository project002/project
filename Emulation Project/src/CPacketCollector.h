/*
 * CPacketCollector.h
 *
 *  Created on: Dec 17, 2013
 *      Author: root
 */

#ifndef CPACKETCOLLECTOR_H_
#define CPACKETCOLLECTOR_H_
#include "BasicIncludes.h"
#include <boost/signals2/mutex.hpp>
#define DEFAULT_BUFFER_SIZE_UNLIMITED 1024

/**
 * Packet collector class, to handle the
 */
class CPacketCollector
{
public:
	/**
	 * Constructor - initializing the buffer size
	 * @param bufferSize the buffer size of the packet collector
	 */
	CPacketCollector(unsigned int bufferSize=DEFAULT_BUFFER_SIZE_UNLIMITED);

	/**
	 * D-tor
	 */
	virtual ~CPacketCollector();

	/**
	 * Adding a packet to the router's buffer
	 * @param pkt packet to be added
	 */
	void PushBack(Crafter::Packet * pkt);

	/**
	 * Popping the packet from the from of the router's buffer
	 * @return if empty NULL is returned
	 */
	Crafter::Packet * PopFront(double & popTime);

	/**
	 * Each router can fill its buffer with fake packets, the function fixes the buffer level percentage to
	 * the wanted fillage.
	 * @param Fillage wanted fill percentage
	 */
	void FixBufferFillage(double Fillage);

	/**
	 * Dropping of random packet from buffer
	 */
	void DropRandomPacket();

	/**
	 * Adding fake packets to the buffer according to the buffer level of a router or
	 * its initial buffer level
	 * @param numberOfPackets total int number of packets to be inserted
	 */
	void AddRandomPackets(unsigned int numberOfPackets);
private:

	/**
	 * Debugging abilities printing of packet
	 */
	void PrintPacket();

	/**
	 * Buffer list of packet received by the packet collector
	 */
	list<Crafter::Packet * > mPackets;

	/**
	 * Receive time of each packet for faster logging abilities to the emulation report
	 */
	list<double> mPacketsReceiveTime;

	/**
	 * Total buffer size of the router
	 */
	unsigned int mBufferSize;

	boost::signals2::mutex mMtx;
};

#endif /* CPACKETCOLLECTOR_H_ */
