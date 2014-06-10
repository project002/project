/*
 * Emulation.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#include "CEmulation.h"
#include "DParserDefs.h"
#define TABLE_SWAPPING_INTERVALS 15

/**
 * Class C-tor - Initiating members
 */
CEmulation::CEmulation(): mPhysicalConnectionsHandler(new CPhysicalConnectionsHandler()),
						  mVirtualRouters(vector<CRouter*>()),
					      mStaticRoutingTable(false),
					      mThreaded(true)
{
	try
	{
		mRunning = false;
	}
	catch(CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

/**
 * Class D-tor - deleting allocated pointers and clearing vectors.
 */
CEmulation::~CEmulation()
{
	try
	{
		vector<CRouter *>::iterator iter;
		for (iter=mRouters.begin();iter!=mRouters.end();iter++)
		{
			//if the router is threaded stopEmulation => interrupt the threads
			if (!(*iter)->isVirtualRouter() && !mThreaded) {(*iter)->StopEmulation();}
			//if all the routers are threaded (mThreaded) stop them all!
			else {(*iter)->StopEmulation();}

		}
		//this will kill all the other nonThreaded routers
		if (!mThreaded) {mRunVirtualRouters.interrupt();}

		if (mPhysicalConnectionsHandler != NULL)
		{
			delete mPhysicalConnectionsHandler;
			mPhysicalConnectionsHandler=NULL;
		}

	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

/**
 * The function calls the physical connections handler to create all physical
 * connections and calling xml parser to build the emulation
 *
 * @param SetupFile XML file which contains the network topology wanted by the researcher
 */
void CEmulation::EmulationBuilder(const char* SetupFile)
{
	try
	{
		mPhysicalConnectionsHandler->CreatePhyiscalConnections();
		XMLParser(SetupFile);
	}
	catch(CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

int CEmulation::routerIndexByID(unsigned int routerID)
{
	for (int unsigned i=0;i<mRouters.size();++i)
	{
		if (mRouters[i]->GetRouterNumber() == routerID) {return i;}
	}
	return NOT_FOUND;
}

void CEmulation::updateFillage(unsigned int routerID, int fillage)
{
	int index = routerIndexByID(routerID);
	if (index != NOT_FOUND) {mRouters[index]->SetFillage(fillage);}
}

void CEmulation::updateDropRate(unsigned int routerID, int dropRate)
{
	int index = routerIndexByID(routerID);
	if (index != NOT_FOUND) {mRouters[index]->SetDropRate(dropRate);}
}


void CEmulation::ToggleRouterActivity(unsigned int routerID,const bool active)
{
	int index = routerIndexByID(routerID);
	if (index != NOT_FOUND)
	{
		mRouters[index]->setRouterAlive(active);
		EmptyRoutingTables();
	}
}

void CEmulation::EmptyRoutingTables()
{
	try
	{
		tableSwappingMTX.lock();
		vector<CRouter *>::iterator iter;
		for (int unsigned i = 0; i < mRouters.size(); ++i)
		{
			for (iter = mRouters.begin(); iter != mRouters.end(); iter++)
			{
				(*iter)->EmptyTables();
			}
		}
		tableSwappingMTX.unlock();
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

/**
 * An iterator goes through the Routers vector,
 * each router then requests the routing tables from all of his connections.
 * The function will be called when table swapping option is enabled according to the
 * XML file and then, it wlil be recalled every minute as the real Routers do.
 */
void CEmulation::TableSwapping()
{
	try
	{
		while(mRunning)
		{
			tableSwappingMTX.lock();
			vector<CRouter *>::iterator iter;
			boost::this_thread::interruption_point();
			//swap table each time for all the connection available
			//(for n routers there are n-1 connections) so the arp responses
			//will occur after one table swap instead of n-1 swaps
			for (int unsigned i=0;i<mRouters.size();++i)
			{
				boost::this_thread::interruption_point();
				for (iter = mRouters.begin(); iter != mRouters.end(); iter++)
				{

					boost::this_thread::interruption_point();
					(*iter)->RequestTables();

				}
			}
			tableSwappingMTX.unlock();
			//Following lines are for the table swapping to take effect every X period
			//of time.
			boost::posix_time::time_duration interval(
					boost::posix_time::seconds(TABLE_SWAPPING_INTERVALS));
			boost::posix_time::ptime timer =
					boost::posix_time::microsec_clock::local_time() + interval;

			boost::this_thread::sleep(
					timer - boost::posix_time::microsec_clock::local_time());
		}
	}
	catch(CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

/**
 * Function adds physical connections to the router according to the XML children.
 *
 * @param router pointer to currently handles router
 * @param iter a reference to the xml parser iterator
 */
void CEmulation::XMLAddPhysicalConnectionsToRouter(CRouter * &router,pugi::xml_node & iter)
{
	try
	{
		// Get router physical connection by its name and add it to the router connections
		for (pugi::xml_node physicalRouterIter = iter.child(
		XML_LAYER_4_INDIVIDUAL_ROUTERS_PHYSICAL_CONNECTION); physicalRouterIter;
				physicalRouterIter = physicalRouterIter.next_sibling(
				XML_LAYER_4_INDIVIDUAL_ROUTERS_PHYSICAL_CONNECTION))
		{
			const char * PhysicalConnectionName = string(
					physicalRouterIter.child_value()).c_str();
			const CPhysicalConnection * connection =
					mPhysicalConnectionsHandler->GetPhysicalConnectionByName(
							PhysicalConnectionName);
			if (connection == NULL)
			{
				throw(CException(
				ERROR_MSG_ADDING_PHYSICAL_CONNECTION_TO_ROUTER));
			}
			router->AddConnection(connection);
		}
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

/**
 * Parsing the routers configuration:
 * -Getting the router buffer size
 * -Creating a router
 * -Adding physical connections to its connections list
 * -Adding virtual connections to its connections list
 * -TBD: What else should be added?
 *
 * @param doc Setup XML tree
 */
void CEmulation::XMLRoutersParser(pugi::xml_document & doc)
{
	try
	{

		CRouter * RouterCreate = NULL;
		pugi::xml_node Routers = doc.child(XML_LAYER_1_NETWORK).child(XML_LAYER_2_ROUTERS);

		for (pugi::xml_node currentRouter = Routers.child(XML_LAYER_3_INDIVIDUAL_ROUTERS); currentRouter;
				currentRouter = currentRouter.next_sibling(XML_LAYER_3_INDIVIDUAL_ROUTERS))
		{
			RouterCreate = new CRouter();

			XMLAddPhysicalConnectionsToRouter(RouterCreate,currentRouter);

			unsigned int RouterNumber = currentRouter.attribute(XML_ROUTER_NUMBER_ATTRIBUTE).as_int();
			RouterCreate->SetRouterNumber(RouterNumber);
			list<CVirtualConnection const *> virtualConnectionsVector=mPhysicalConnectionsHandler->GetVirtualConnectionsVector(RouterNumber);
			RouterCreate->AppendConnectionList(virtualConnectionsVector);
			//Get router buffer size default is defined in H file
			unsigned int BufferSize = currentRouter.attribute(XML_ROUTER_BUFFER_SIZE_ATTRIBUTE).as_int();
			if (BufferSize != 0)
			{
				RouterCreate->SetBufferSize(BufferSize);
			}
			double DropRate = currentRouter.attribute(
					XML_ROUTER_DROP_RATE_ATTRIBUTE).as_double();

			RouterCreate->SetDropRate(DropRate);

			unsigned int BufferUsedSize = currentRouter.attribute(XML_ROUTER_INITIAL_USED_BUFFER_SIZE_ATTRIBUTE).as_int();
			if(BufferUsedSize!=0)
			{
				RouterCreate->SetInitialBufferUse(BufferUsedSize);
			}
			double Fillage = currentRouter.attribute(
					XML_ROUTER_FILLAGE_ATTRIBUTE).as_double();

			RouterCreate->SetFillage(Fillage);

			string fillageArray = currentRouter.attribute(
					XML_ROUTER_DYNAMIC_FILLAGE_ARRAY_ATTRIBUTE).as_string();

			RouterCreate->SetDynamicFillageArray(fillageArray);

			string dropRateArray = currentRouter.attribute(
			XML_ROUTER_DYNAMIC_DROP_RATE_ARRAY_ATTRIBUTE).as_string();
			RouterCreate->SetDynamicDropRateArray(dropRateArray);

			SReport::getInstance().LogRouter(RouterNumber,BufferSize,DropRate,BufferUsedSize,Fillage);

			//TODO: add to gui the init buffer size and fillage rate(in percent)
			SDataController::getInstance().msg("Created Router %d :: Buffer Of %d Packets :: DropRate %.1f%%",RouterNumber,BufferSize,DropRate);
			mRouters.push_back(RouterCreate);
		}
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

/**
 * Parsing the virtual connections:
 *
 * @param doc Setup XML tree
 */
void CEmulation::XMLVirtualConnectionsParser(pugi::xml_document & doc)
{
	try
	{
		pugi::xml_node VirtualConnections = doc.child(XML_LAYER_1_NETWORK).child(XML_LAYER_2_VIRTUAL_CONNECTIONS);

		CVirtualConnection * virtualCreate;
		for (pugi::xml_node iter = VirtualConnections.child(XML_LAYER_3_INDIVIDUAL_VIRTUAL_CONNECTIONS); iter;
				iter = iter.next_sibling(XML_LAYER_3_INDIVIDUAL_VIRTUAL_CONNECTIONS))
		{
			virtualCreate=new CVirtualConnection();
			for (pugi::xml_node virtualConnectionIter = iter.child(
					XML_LAYER_4_INDIVIDUAL_VIRTUAL_CONNECTIONS_ROUTER_NUMBER); virtualConnectionIter;
					virtualConnectionIter = virtualConnectionIter.next_sibling(
							XML_LAYER_4_INDIVIDUAL_VIRTUAL_CONNECTIONS_ROUTER_NUMBER))
			{
				unsigned int RouterNumber = virtualConnectionIter.attribute(XML_ROUTER_NUMBER_ATTRIBUTE).as_int();
				virtualCreate->AddInvolvedRouter(RouterNumber);
			}
			mPhysicalConnectionsHandler->addVirtualConnection(virtualCreate);
		}
	}
	catch(CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

/**
 * Parsing the routing table if its provided by the user in the XML
 * otherwise the table swapping algorithm will be in charge of getting
 * the routing tables for each router.
 * @param doc Setup XML tree
 */
void CEmulation::XMLRoutingTableParserAvailability(pugi::xml_document & doc)
{
	try
	{
		//after consulting with Martin, there is no need for manual table building
		mStaticRoutingTable= doc.child(XML_LAYER_1_NETWORK).child(XML_LAYER_2_IS_STATIC_TABLE).attribute(XML_STATIC_LAYER_ATTRIBUTE).as_bool();
		mTableSwappingThread = boost::thread(&CEmulation::TableSwapping, this);
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

void CEmulation::XMLThreadedOptionParse(pugi::xml_document & doc)
{
	pugi::xml_node root = doc.child(XML_LAYER_1_NETWORK);
	mThreaded = root.attribute(THREADED_EMULATION).as_bool(true); //default to true
	if (mThreaded)
	{SDataController::getInstance().msg("Emulation Running High-End Setting");}
	else
	{SDataController::getInstance().msg("Emulation Running Low-End Setting");}
}

/**
 * Parsing the XML file provided.
 * -Loading the file == Checking its validity
 * -Creating virtual connections
 * -Creating routers with relevant connections
 * -Building the routing table.
 *
 * @param SetupFile XML file which contains the network topology wanted by the researcher
 */
void CEmulation::XMLParser(const char * SetupFile)
{
	try
	{
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(SetupFile);
		if (!result)
		{
			throw(CException(ERROR_MSG_XML_PARSER));
		}
		XMLThreadedOptionParse(doc);
		XMLVirtualConnectionsParser(doc);
		XMLRoutersParser(doc);
		XMLRoutingTableParserAvailability(doc);
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

/**
 * Starting the emulation itself after the initiation phases.
 * Each router starts sniffing and handling his own traffic.
 *
 */
void CEmulation::StartEmulation()
{
	try
	{
		vector<CRouter *>::iterator iter;
		mRunning = true;
		//STARTing sniffer on all routers
		for (iter=mRouters.begin();iter!=mRouters.end();iter++)
		{
			if (!mThreaded && (*iter)->isVirtualRouter())
			{
				mVirtualRouters.push_back(*iter);
				(*iter)->nonThreadedInit();
			}
			else
			{
				(*iter)->Sniffer();
			}
		}
		if (!mThreaded)
		{
			//if not threaded make the thread to run all the routers
			SLogger::getInstance().Logf("Running Virtual Routers Thread With %d Routers",mVirtualRouters.size());
			mRunVirtualRouters = boost::thread(&CEmulation::virtualRoutersSequence,this);
		}

		while(true)
		{
			//keep busy
			runningUpdMTX.lock();
			if (!mRunning) {break;}
			runningUpdMTX.unlock();
		}

		if(!mStaticRoutingTable)
		{
			mTableSwappingThread.interrupt();
		}

	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw error;
	}
}

void CEmulation::StopEmulation()
{
	runningUpdMTX.lock();
	mRunning = false;
	runningUpdMTX.unlock();
}


void CEmulation::virtualRoutersSequence()
{
	if (mThreaded) {return;}
	vector<CRouter *>::iterator it;
	while (mRunning)
	{
		boost::this_thread::interruption_point();
		it = mVirtualRouters.begin();
		for (;it!=mVirtualRouters.end();++it)
		{
			(*it)->nonThreadedSniffer();
		}
	}
}
