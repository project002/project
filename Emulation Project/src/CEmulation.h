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
#include "SDataController.h"
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
	void StopEmulation();
	void EmulationBuilder(const char* SetupFile);

private: // Private Functions
	/**
	 * Flag whether or not the emulation is running
	 */
	bool mRunning;

	/**
	 * XML Parsing functions.
	 * -Router Parser
	 * -Connections Parser
	 * -Routing Table
	 * -Threaded emulation or not
	 * @param SetupFile
	 */
	void XMLParser(const char * SetupFile);
	void XMLRoutersParser(pugi::xml_document & doc);
	void XMLAddPhysicalConnectionsToRouter(CRouter * &router,pugi::xml_node & iter);
	void XMLVirtualConnectionsParser(pugi::xml_document & doc);
	void XMLRoutingTableParserAvailability(pugi::xml_document & doc);
	void XMLThreadedOptionParse(pugi::xml_document & doc);

	/**
	 * When not using the threaded emulation option, the virtual routers are working
	 * sequentially, for better performance on low end devices
	 */
	void virtualRoutersSequence();

	/**
	 * Table swapping occurs every TABLE_SWAPPING_INTERVALS seconds
	 */
	void TableSwapping();

private:
	//Private Members
	CPhysicalConnectionsHandler * mPhysicalConnectionsHandler;
	//Emulation routers
	vector<CRouter *> mRouters;
	//Emulation virtual routers for sequential flow on not threaded emulations
	vector<CRouter *> mVirtualRouters; //used when not threaded
	bool mStaticRoutingTable;
	//Is emulation threaded or not
	bool mThreaded;
	//Threading and MUTEX to run simultaneously
	boost::thread mTableSwappingThread;
	boost::thread mRunVirtualRouters;
	boost::signals2::mutex runningUpdMTX;
};

#endif /* CEMULATION_H_ */
