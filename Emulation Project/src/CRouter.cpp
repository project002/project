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
		for(iter = mConnections.begin();iter!=mConnections.end();iter++)
		{
			(*iter)->GetTable();
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

