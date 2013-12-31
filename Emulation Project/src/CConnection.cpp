/*
 * Connection.cpp
 *
 *  Created on: Dec 31, 2013
 *      Author: root
 */

#include "CConnection.h"

CConnection::CConnection():mSocket(-1)
{
	// TODO Auto-generated constructor stub

}
Crafter::Packet* CConnection::GetPacket(){
		try
		{
			throw(CException("Missing Implementation"));
		}
		catch (CException & error)
		{
			std::cerr << error.what() << std::endl;
			std::cerr << __PRETTY_FUNCTION__ << std::endl;
			throw;
		}
		return NULL;
	}
CConnection::~CConnection()
{
	// TODO Auto-generated destructor stub
}

