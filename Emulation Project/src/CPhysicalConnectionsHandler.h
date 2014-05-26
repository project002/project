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
#include "CVirtualConnection.h"
/**
 * The class handles the physical connections of the device, as well as the "virtual"
 * including opening sockets, connecting, and closing.
 */
class CPhysicalConnectionsHandler
{
public:
	/**
	 * Constructor
	 */
	CPhysicalConnectionsHandler();

	/**
	 * Destructor - deleting the physical and virtual connections
	 */
	virtual ~CPhysicalConnectionsHandler();

	/**
	 * Creating all the physical connections of the emulation server
	 */
	void CreatePhyiscalConnections();

	/**
	 * Adding the virtual connections the the handler to delete them when needed
	 * @param virtualConnection
	 */
	void addVirtualConnection(CVirtualConnection * virtualConnection){mVirtualConnections.push_back(virtualConnection);}

	/**
	 * Returning the full list of virtual connections
	 * @param routerNumber
	 * @return
	 */
	list<CVirtualConnection const *> GetVirtualConnectionsVector(const unsigned int routerNumber);

	/**
	 * Returning reference to a physical connection class by its name
	 * @param InterfaceName the physical connection's name
	 * @return the physical connection itself
	 */
	CPhysicalConnection const * GetPhysicalConnectionByName(const char * InterfaceName) ;
private: // Private Functions

private: // Private Members
	vector<CVirtualConnection *> mVirtualConnections;
	vector<CPhysicalConnection *>  mPhysicalConnections;
};

#endif /* CPHYSICALCONNECTIONSHANDLER_H_ */
