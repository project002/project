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
#include "SBasicGUI.h"
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
	void EmulationBuilder(char* SetupFile);

private: // Private Functions

	void XMLParser(char * SetupFile);
	void XMLRoutersParser(pugi::xml_document & doc);
	void XMLAddPhysicalConnectionsToRouter(CRouter * &router,pugi::xml_node & iter);
	void XMLVirtualConnectionsParser(pugi::xml_document & doc);
	void XMLRoutingTableParserAvailability(pugi::xml_document & doc);
	void XMLParseRoutingTable(pugi::xml_document & doc);

	void TableSwapping();
private: //Private Members
	CPhysicalConnectionsHandler * mPhysicalConnectionsHandler;
	vector<CRouter *> mRouters;
	bool mStaticRoutingTable;
	boost::thread mTableSwappingThread;
};

#endif /* CEMULATION_H_ */
