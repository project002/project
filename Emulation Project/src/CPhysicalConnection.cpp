/*
 * CPhysicalConnection.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#include "CPhysicalConnection.h"

CPhysicalConnection::CPhysicalConnection(struct ifaddrs* device)
{
	//read relevant data from stuct ifaddr and creates the appropriate
	//socket type?
	try
	{
		mSocket = socket();
		if (mSocket == -1) {throw CEmulation("socket is bad");}
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}

}

CPhysicalConnection::~CPhysicalConnection()
{
	// TODO Auto-generated destructor stub
}

