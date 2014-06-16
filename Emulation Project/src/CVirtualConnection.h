/*
 * CVirtualConnection.h
 *
 *  Created on: Dec 31, 2013
 *      Author: root
 */

#ifndef CVIRTUALCONNECTION_H_
#define CVIRTUALCONNECTION_H_
//Base class
#include "CConnection.h"
//Packet collectors
#include "CPacketCollector.h"
//
#include <boost/signals2/mutex.hpp>
#define NUMBER_OF_CONNECTED_DEVICES 2
#include <algorithm>

/**
 * A connection class of a virtual connection
 */
class CVirtualConnection: public CConnection
{
public:

	/**
	 * Constructor - determining the MAC address of the connection to be set
	 */
	CVirtualConnection();

	/**
	 * Retrieving of packets from a virtual connection
	 * @param routerNumber Virtual connections has 2 virtual routers attached to them, in which the correct packet collector should be chosen
	 * @return first packet in the packet collector's FIFO
	 */
	virtual Crafter::Packet* GetPacket(int routerNumber=NO_CHOICE_OF_PACKET_COLLECTOR);

	/**
	 * Sending of packets from a virtual connection
	 * @param packet The packet to be sent
	 * @param routerNumber Virtual connections has 2 virtual routers attached to them, in which the correct packet collector should be chosen
	 */
	virtual void SendPacket(Packet* packet,int routerNumber=NO_CHOICE_OF_PACKET_COLLECTOR);

	/**
	 * Getter of the connection's IPv4 address
	 * @return
	 */
	CUIPV4*& getGetwayAddress() {return mUniqueIPForConnection;}

	/**
	 * Getter of the connection's MAC address
	 * @return
	 */
	virtual string GetMAC()const{return mMacAddress;};

	/**
	 * Each virtual connection holds 2 routers, when sending and receiving packets they should move from
	 * one router to the other, therefore the router numbers are being saved
	 * @param routerNumber involved router in this virtual connection
	 */
	void AddInvolvedRouter(const unsigned int & routerNumber);

	/**
	 * Verifying if a certain router is using the current connection
	 * @param routerNumber router number to be checked
	 * @return true if the router is using this conneciton
	 */
	bool IsInvolved(const unsigned int &routerNumber){return std::find(mInvolvedRoutersByNumber.begin()
			, mInvolvedRoutersByNumber.end(), routerNumber)!=mInvolvedRoutersByNumber.end();}

	/**
	 * D-tor - freeing the unique ip given to the connection.
	 */
	virtual ~CVirtualConnection();

	/**
	 * Connection receives a map of connections he can reach and adds them to its own table
	 * for future use in the router's forwarding rules
	 * @param routTable table with entries that should be added to current connection table
	 * @param routerNumber
	 */
	void AddRoutingTableReference(map<string,pair<CConnection const*,string> > * routTable, int routerNumber);

	/**
	 * Retrieving the routing tables from each connection type
	 * @return pairs vector of IPv4 and MAC addresses achievable by the current connection
	 */
	virtual vector<pair<string, string> >& GetTable() ;

	/**
	 * Checking if the current connection is physical or virtual, saving time on dynamic casting
	 * since it is needed in order to determine the report finishing of each packet
	 * @return true if the connection is physical
	 */
	bool isPhysical()const{return false;}

	/**
	 * clearing the routing tables
	 */
	virtual void ClearTables();
private:

	/**
	 * Routing table to be returned
	 */
	vector<pair<string, string> > mRoutingToReturn;

	/**
	 * Each virtual connection holds 2 routers, when sending and receiving packets they should move from
	 * one router to the other, therefore the router numbers are being saved
	 */
	vector<unsigned int> mInvolvedRoutersByNumber;

	/**
	 * Each virtual connection is "connected" to two routers and therefore holds two packet collectors
	 */
	CPacketCollector mPacketCollectors[NUMBER_OF_CONNECTED_DEVICES];

	/**
	 * a switch like map to define which router sends and receives from which packet collector
	 */
	map<unsigned int ,unsigned int > mRouterToPacketCollectorMap;

	//MAP < IP , <Connection,MAC> > reference
	/**
	 * Routing table for each router connected
	 */
	map<string,pair<CConnection const*,string> > * mRoutingTable[NUMBER_OF_CONNECTED_DEVICES];

	/**
	 * Combination of retrieved routing tables from each router into one big unique set
	 */
	set< string > mCombinedRoutingTable;

	/**
	 * virtual connections MAC adderss
	 */
	string mMacAddress;

	/**
	 * "Unique" IP per virtual connection
	 */
	CUIPV4 *mUniqueIPForConnection;

	/**
	 * Virtual connection ID
	 */
	int unsigned id; //the id of this virtual connection

	/**
	 * Table handling MUTEX since 2 routers handle it simultaneously
	 */
	boost::signals2::mutex mMtx;

	/**
	 * Determining the connection's MAC address by an increasing addresses
	 * @return
	 */
	string makeSequentialMACAddress();

};

#endif /* CVIRTUALCONNECTION_H_ */
