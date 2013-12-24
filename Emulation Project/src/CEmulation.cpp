/*
 * Emulation.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#include "CEmulation.h"

CEmulation::CEmulation(): mPhysicalConnectionsHandler(NULL)
{
	try
	{
		InitEmulation();
	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

CEmulation::~CEmulation()
{
	try
	{
		if (mPhysicalConnectionsHandler != NULL)
		{
			delete mPhysicalConnectionsHandler;
			mPhysicalConnectionsHandler=NULL;
		}
		vector<CPhysicalConnection *>::iterator it =
				mPhysicalConnections.begin();
		for (; it != mPhysicalConnections.end(); it++)
		{
			delete (*it);
		}
		mPhysicalConnections.clear();
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

void CEmulation::InitEmulation()
{
	try
	{
		mPhysicalConnectionsHandler = new CPhysicalConnectionsHandler();
		mPhysicalConnectionsHandler->CreatePhyiscalConnections();
		//TODO ping all subnet addresses to check who is connected
	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

void CEmulation::EmulationBuilder()
{
	try
	{
		//TODO : read xml to get emulation details - connections / routers / etc...
	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

