/*
 * CRouter.h
 *
 *  Created on: Dec 31, 2013
 *      Author: root
 */

#ifndef CROUTER_H_
#define CROUTER_H_
#include "BasicIncludes.h"
#include "CConnection.h"
#include "CUIPV4.h"
#include "CPacketCollector.h"
/**
 * Thread Related Inclusions
 */
#include <boost/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

#define DEFAULT_ROUTER_BUFFER_SIZE 100
class CRouter
{
public:
	CRouter();
	virtual ~CRouter();
	void AddConnection(const CConnection * connection){mConnections.push_back(connection);}
	void RequestTables();
	void Sniffer();

	unsigned int GetBufferSize() const
	{
		return mBufferSize;
	}

	void SetBufferSize(unsigned int bufferSize)
	{
		mBufferSize = bufferSize;
	}

private:
	void Sniff();
	void PacketHandler();
	void ProcessSendPakcet(Packet* packet);
	list<CConnection const *> mConnections;
	//holds tuples of threes
	//1 - the ip address
	//2 - the connection to send for this ip address
	//3 - the MAC address of the next hop for this ip address
	map<string,pair<CConnection const*,string> > mRoutingTable;
	unsigned int mBufferSize;
	boost::thread mSniffingThread;
	boost::thread mPacketsHandlingThread;
	CPacketCollector * mPacketCollector;

};

#endif /* CROUTER_H_ */
