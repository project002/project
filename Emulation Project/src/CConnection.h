/*
 * Connection.h
 *
 *  Created on: Dec 31, 2013
 *      Author: root
 */

#ifndef CCONNECTION_H_
#define CCONNECTION_H_
#include "BasicIncludes.h"
#define NO_CHOICE_OF_PACKET_COLLECTOR -1

/**
 * Base class for physical and virtual connections
 */
class CConnection
{
public:
	/**
	 * Constructor, initializing the socket number
	 */
	CConnection();

	/**
	 * Retrieving of packets from a virtual connection or a physical connection
	 * @param routerNumber Virtual connections has 2 virtual routers attached to them, in which the correct packet collector should be chosen
	 * 		  this value in a physical connection is ignored
	 * @return first packet in the packet collector's FIFO
	 */
	virtual Crafter::Packet* GetPacket(int routerNumber=NO_CHOICE_OF_PACKET_COLLECTOR) {throw CException("missing implementation of GetPacket in cconnection");}

	/**
	 * Getter of the connection's IPv4 address
	 * @return
	 */
	virtual CUIPV4*& getGetwayAddress() {throw CException("missing implementation of get gateway in cconnection");}

	/**
	 * Retrieving the routing tables from each connection type
	 * @return pairs vector of IPv4 and MAC addresses achievable by the current connection
	 */
	virtual vector< pair<string,string> >& GetTable(){throw(CException("Impelement get table in connections"));}

	/**
	 * Sending of packets from a virtual connection or a physical connection
	 * @param packet The packet to be sent
	 * @param routerNumber Virtual connections has 2 virtual routers attached to them, in which the correct packet collector should be chosen
	 * 		  this value in a physical connection is ignored
	 */
	virtual void SendPacket(Packet* packet,int routerNumber=NO_CHOICE_OF_PACKET_COLLECTOR){throw(CException("Implement sendPacket in connections"));}

	/**
	 * Getter of the connection's MAC address
	 * @return
	 */
	virtual string GetMAC()const{throw(CException("Implement GetMAC in connections"));};

	/**
	 * D-tor
	 */
	virtual ~CConnection();

	/**
	 * Checking if the current connection is physical or virtual, saving time on dynamic casting
	 * since it is needed in order to determine the report finishing of each packet
	 * @return true if the connection is physical
	 */
	virtual bool isPhysical()const=0;

	virtual void ClearTables(){}
protected:
	int mSocket;
};

#endif /* CCONNECTION_H_ */
