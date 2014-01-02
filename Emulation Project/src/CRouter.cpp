/*
 * CRouter.cpp
 *
 *  Created on: Dec 31, 2013
 *      Author: root
 */

#include "CRouter.h"

CRouter::CRouter():mBufferSize(DEFAULT_ROUTER_BUFFER_SIZE)
{
	try
	{


	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}

}
void CRouter::RequestTables()
{
	try
	{
		list<CConnection *>::iterator iter;
		//iterate over all connections
		for(iter = mConnections.begin();iter!=mConnections.end();iter++)
		{
			//iterate over all ips in the table you got from the connection
			vector<string>& tables=(*iter)->GetTable();
			vector<string>::iterator it=tables.begin();
			for (;it!=tables.end();it++)
			{
				mRoutingTable.insert(pair<string,CConnection*>((*it),(*iter)));
				cout<< (*it)<<endl;
			}
		}
	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}
CRouter::~CRouter()
{
	try
	{
		mConnections.clear();
	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}

