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
#include "CVirtualConnection.h"
#include "CPhysicalConnectionsHandler.h"
#include "pugixml.hpp"
#include "CRouter.h"
/**
 * Thread Related Inclusions
 */
#include <boost/thread.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
class CEmulation
{
public: // Public Functions

	CEmulation();
	virtual ~CEmulation();
	void StartEmulation();
	/**
	 * Start building the network to be emulated.
	 */
	void EmulationBuilder(char* SetupFile);

private: // Private Functions

	/**
	 * Opening sockets for the physical Ethernet ports
	 */
	void InitEmulation();
	/**
	 * parsing the setup information - routers connections buffer sizes etc
	 * @param SetupFile
	 */
	void XMLParser(char * SetupFile);
	void XMLRoutersParser(pugi::xml_document & doc);
	void XMLVirtualConnectionsParser(pugi::xml_document & doc);
	void XMLRoutingTableParser(pugi::xml_document & doc);
	void XMLParseRoutingTable(pugi::xml_document & doc);

	void TableSwapping();
private: //Private Members
	CPhysicalConnectionsHandler * mPhysicalConnectionsHandler;
	vector<CPhysicalConnection * >  mPhysicalConnections;
	vector<CRouter *> mRouters;
	bool mStaticRoutingTable;
};

#endif /* CEMULATION_H_ */
