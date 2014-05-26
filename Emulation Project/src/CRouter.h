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
#include "CVirtualConnection.h"
/**
 * Thread Related Inclusions
 */
#include <boost/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/signals2/mutex.hpp>
#define DEFAULT_ROUTER_BUFFER_SIZE 100
#define MAX_FILLAGE_RATE 100
#define MAX_DROP_RATE 7
#define MAX_INTERVAL_BETWEEN_FILLAGE_CHANGE 100
class CRouter
{
public:
	/**
	 * Constructor - initializing the members
	 */
	CRouter();

	/**
	 * Deleting the allocated members
	 */
	virtual ~CRouter();

	/**
	 * Add the connections related to the router
	 * @param connection
	 */
	void AddConnection(const CConnection * connection){mConnections.push_back(connection);}

	/**
	 * Add the virtual connections connected to the router
	 * @param connectionList vector of connections
	 */
	void AppendConnectionList(list<CVirtualConnection const *> &connectionList);

	/**
	 * Requests tables from its connections in order to build its routing table
	 */
	void RequestTables();

	/**
	 * Threaded sniffing and packer handling
	 */
	void Sniffer();

	/**
	 * Non threaded emulation functions
	 */
	void nonThreadedSniffer();
	void nonThreadedInit();

	/**
	 * Stopping threads
	 */
	void StopEmulation();

	/**
	 * Setters and getters of the router properties
	 * @return
	 */
	unsigned int GetBufferSize() const
	{
		return mBufferSize;
	}

	void SetBufferSize(unsigned int bufferSize)
	{
		mBufferSize = bufferSize;
	}

	double GetDropRate() const
	{
		return mDropRate;
	}

	void SetDropRate(double dropRate)
	{
		mDropRate = dropRate;
	}

	unsigned int GetRouterNumber() const
	{
		return mRouterNumber;
	}

	void SetRouterNumber(unsigned int routerNumber)
	{
		mRouterNumber = routerNumber;
	}

	void AddPacketsToBuffer(unsigned int numberOfPackets=0)
	{
		mPacketCollector->AddRandomPackets(numberOfPackets);
	}

	void setRouterThreaded(bool threaded) {mThreaded = threaded;}

	/**
	 * this method determines if the router is comprised out of
	 * virtual connection only
	 *  NIEN //Note: this method uses RTTI so use it in the init only
	 * @return
	 */
	bool isVirtualRouter()
	{
//		//check if all connection are virtual =>| this router is virtual
//		list<CConnection const *>::iterator it = mConnections.begin();
//		CVirtualConnection const * cvc;
//		for (;it!=mConnections.end();++it)
//		{
//
//			cvc = dynamic_cast<CVirtualConnection const *>(*it);
//			if (cvc == NULL) {return false;}
//		}
//		return true; //TODO: changed flow -GAL- to isPhysical
		//check if all connection are virtual =>| this router is virtual
		list<CConnection const *>::iterator it = mConnections.begin();
		for (; it != mConnections.end(); ++it)
		{
			if((*it)->isPhysical())
			{
				return false;
			}
		}
		return true;
	}

	double GetFillage() const
	{
		return mFillage;
	}

	void SetFillage(double fillage)
	{
		mFillage = fillage;
	}

	unsigned int GetInitialBufferUse() const
	{
		return mInitialBufferUse;
	}

	void SetInitialBufferUse(unsigned int initialBufferUse)
	{
		mInitialBufferUse = initialBufferUse;
	}

	/**
	 * Dynamic properties such as fillage and drop rate percentage
	 * @param str
	 */
	void SetDynamicFillageArray(string str);
	void SetDynamicDropRateArray(string str);
private:
	/**
	 * Packet handlers
	 * @param pkt packet to be handled
	 */
	void PacketHandler();
	void HandleArp(Packet * pkt);
	void HandleIPv4(Packet * pkt,const double popTime);
	bool ProcessSendPacket(Packet* packet);

	/**
	 * Sniff packets
	 */
	void Sniff();

	/**
	 * Dynamic Properties handlers
	 */
	void DynamicFillageHandler();
	void DynamicDropRateHandler();

private://members
	//Connections by the routers use
	list<CConnection const *> mConnections;
	//MAP < IP , <Connection,MAC> > routing table
	map<string,pair<CConnection const*,string> > mRoutingTable;

	//router properties
	unsigned int mBufferSize;
	double mDropRate;
	double mFillage;
	unsigned int mInitialBufferUse;
	unsigned int mRouterNumber;

	//Thread abilities
	boost::thread mSniffingThread;
	boost::thread mPacketsHandlingThread;

	//Packet collector
	CPacketCollector * mPacketCollector;

	bool mThreaded; //determins if the router is threaded or not
	boost::signals2::mutex mMtx;

	//dynamic fillage handles ( buffer leve )
	vector< pair< double,double > > mFillageArr;
	vector< pair< double,double > >::iterator mFillageArrIterator;
	//dynamic drop rate
	vector< pair< double,double > > mDropRateArr;
	vector< pair< double,double > >::iterator mDropRateArrIterator;
};

#endif /* CROUTER_H_ */
