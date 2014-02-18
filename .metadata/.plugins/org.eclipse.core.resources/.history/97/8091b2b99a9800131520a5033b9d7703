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
class CConnection
{
public:
	CConnection();
	virtual Crafter::Packet* GetPacket(int routerNumber=NO_CHOICE_OF_PACKET_COLLECTOR) {throw CException("missing implementation of GetPacket in cconnection");}
	virtual CUIPV4*& getGetwayAddress() {throw CException("missing implementation of get gateway in cconnection");}
	//TODO: get table in sub classes - physical connections will return addresses according to the DHCP availability table
	// and virtual connections will return according to XML or dynamically populated tables
	virtual vector< pair<string,string> >& GetTable(){throw(CException("Impelement get table in connections"));}
	virtual bool SendPacket(Packet* packet,int routerNumber=NO_CHOICE_OF_PACKET_COLLECTOR){throw(CException("Implement sendPacket in connections"));}
	virtual string GetMAC()const{throw(CException("Implement GetMAC in connections"));};
	virtual ~CConnection();
protected:
	int mSocket;
	//TODO: hold table of "connections" to computers by IP
};

#endif /* CCONNECTION_H_ */
