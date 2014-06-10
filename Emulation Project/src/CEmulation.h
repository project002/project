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
#define NOT_FOUND -1
class CEmulation
{
public: // Public Functions

	/**
	 * C-Tor - initializes the class members
	 */
	CEmulation();

	/**
	 * D-Tor - Deletes all allocated pointers such as routers and physical connection handler
	 * as well as finishing all the running threads of the emulation
	 */
	virtual ~CEmulation();

	/**
	 * Begin the emulation, starting the routers's sniffer
	 */
	void StartEmulation();

	/**
	 * "Stopping" the emulation.
	 * Setting the is emulation running member to false, which triggers the destruction of threads
	 * and finishing the current run.
	 */
	void StopEmulation();

	/**
	 * Creating the physical connections handler and parsing the xml file of the wanted emulation network structure
	 * @param SetupFile xml file to load the wanted emulation network structure from
	 */
	void EmulationBuilder(const char* SetupFile);

	/**
	 * Updating 'routerID' router fillage rate the 'fillage'
	 * @param routerID router number to change
	 * @param fillage
	 */
	void updateFillage(unsigned int routerID,int fillage);

	/**
	 * Updating 'routerID' router drop rate rate the 'dropRate'
	 * @param routerID router number to change
	 * @param dropRate
	 */
	void updateDropRate(unsigned int routerID,int dropRate);

	/**
	 * the functions ability to shut down / revive a router for different path use
	 * @param routerID router number to toggle
	 * @param active boolean to toggle the router to active or not
	 */
	void ToggleRouterActivity(unsigned int routerID,const bool active);

	/**
	 * When a router dies / comes back to life
	 * the routing tables will be emptied in order for the tables to be updated
	 */
	void EmptyRoutingTables();

private: // Private Functions
	/**
	 * Flag whether or not the emulation is running
	 * for the easy destruction of multiple threads
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

	int routerIndexByID(unsigned int routerID);
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
	boost::signals2::mutex tableSwappingMTX;
};

#endif /* CEMULATION_H_ */
