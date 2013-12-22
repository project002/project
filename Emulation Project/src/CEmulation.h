/*
 * Emulation.h
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#ifndef CEMULATION_H_
#define CEMULATION_H_

#include "BasicIncludes.h"
#include "CPhysicalConnection.h"
#include "CPhysicalConnectionsHandler.h"
class CEmulation
{
public: // Public Functions

	CEmulation();
	virtual ~CEmulation();

	/**
	 * Start building the network to be emulated.
	 */
	void EmulationBuilder();

private: // Private Functions

	/**
	 * Opening sockets for the physical Ethernet ports
	 */
	void InitEmulation();



private: //Private Members
	CPhysicalConnectionsHandler * mPhysicalConnectionsHandler;
	vector<CPhysicalConnection * >  mPhysicalConnections;
};

#endif /* CEMULATION_H_ */
