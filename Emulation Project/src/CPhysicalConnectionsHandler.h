/*
 * CPhysicalConnectionsHandler.h
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#ifndef CPHYSICALCONNECTIONSHANDLER_H_
#define CPHYSICALCONNECTIONSHANDLER_H_
#include "BasicIncludes.h"
#include "CPhysicalConnection.h"
#define MAX_NUMBER_OF_COMPUTERS_ON_NET 32
/**
 * The class handles the physical connections of the device,
 * including opening sockets, connecting, and closing.
 *
 * //TODO: Class will loop through all physical connections and create a CPhysicalConnection class for each
 * //TODO: Eventually it will return a vector of pointers for physical connections for the emulation to use.
 */
class CPhysicalConnectionsHandler
{
public:
	CPhysicalConnectionsHandler();
	virtual ~CPhysicalConnectionsHandler();
	void CreatePhyiscalConnections();

private: // Private Functions

private: // Private Members
	vector<CPhysicalConnection * > * mPhysicalConnections;
};

#endif /* CPHYSICALCONNECTIONSHANDLER_H_ */
