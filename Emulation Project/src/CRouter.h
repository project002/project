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
#define DEFAULT_ROUTER_BUFFER_SIZE 100
class CRouter
{
public:
	CRouter();
	virtual ~CRouter();
	void AddConnection(CConnection * connection){mConnections.push_back(connection);}
	void RequestTables();
	void StartSniffing(){};//TODO implement sniffing from all connections
	unsigned int GetBufferSize() const
	{
		return mBufferSize;
	}

	void SetBufferSize(unsigned int bufferSize)
	{
		mBufferSize = bufferSize;
	}

private:
	list<CConnection *> mConnections;
	map<string,CConnection*> mRoutingTable;
	unsigned int mBufferSize;
	//TODO: add list of connections which includes virtual ones and physical ones
};

#endif /* CROUTER_H_ */
