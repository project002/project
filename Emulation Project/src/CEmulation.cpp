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
		throw;
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
		throw;
	}
}

void CEmulation::InitEmulation()
{
	try
	{
		mPhysicalConnectionsHandler = new CPhysicalConnectionsHandler();
		mPhysicalConnectionsHandler->CreatePhyiscalConnections();
	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}

/**
 * Function gets a setup file name and builds the emulation according to it.
 * First adding routers and defining their connections
 * @param SetupFile
 */
void CEmulation::EmulationBuilder(char* SetupFile)
{
	try
	{
		XMLParser(SetupFile);
		TableSwapping();

	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}
/**
 * Table swapping between all routers and physical connections
 */
void CEmulation::TableSwapping()
{
	try
	{
		vector<CRouter *>::iterator iter;
		for (iter=mRouters.begin();iter!=mRouters.end();iter++)
		{
			(*iter)->RequestTables();
		}
	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}
void CEmulation::XMLRoutersParser(pugi::xml_document & doc)
{
	try
	{

		CRouter * RouterCreate = NULL;
		pugi::xml_node Routers = doc.child("Network").child("Routers");

		for (pugi::xml_node iter = Routers.child("Router"); iter;
				iter = iter.next_sibling("Router"))
		{

			RouterCreate = new CRouter();
			//Get router physical connections
			for (pugi::xml_node physicalRouterIter = iter.child(
					"PhysicalConnection"); physicalRouterIter;
					physicalRouterIter = physicalRouterIter.next_sibling(
							"PhysicalConnection"))
			{
				const char * PhysicalConnectionName = string(
						physicalRouterIter.child_value()).c_str();
				CPhysicalConnection * connection =
						mPhysicalConnectionsHandler->GetPhysicalConnectionByName(
								PhysicalConnectionName);
				if (connection == NULL)
				{
					throw(CException(
							"Can't find wanted physical connection, make sure XML is provided with legitimate connection name"));
				}
				RouterCreate->AddConnection(connection);
			}

			//Get router virtual connections

			for (pugi::xml_node virtualRouterIter = iter.child(
					"VirtualConnections"); virtualRouterIter;
					virtualRouterIter = virtualRouterIter.next_sibling(
							"VirtualConnections"))
			{
				CConnection * connection = new CVirtualConnection();
				if (connection == NULL)
				{
					throw(CException(
							"Can't find wanted virtual connection, make sure XML is provided with legitimate connection name"));
				}
				RouterCreate->AddConnection(connection);
			}
			//Get router buffer size default is defined in H file
			unsigned int BufferSize = iter.attribute("BufferSize").as_int();
			if (BufferSize != 0)
			{
				RouterCreate->SetBufferSize(BufferSize);
			}
			mRouters.push_back(RouterCreate);
		}
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}
void CEmulation::XMLVirtualConnectionsParser(pugi::xml_document & doc)
{
	try
	{
		//TODO: create virtual connections according to XML
		//TODO: how do we want to define those virtual connections?
		//TODO: open sockets between 2 routers == threads
		CVirtualConnection * connection = NULL;
		pugi::xml_node VirtualConnections = doc.child("Network").child("VirtualConnections");

		for (pugi::xml_node iter = VirtualConnections.child("VirtualConnections"); iter;
				iter = iter.next_sibling("VirtualConnections"))
		{
			//Get virtual connections
			//TODO: need to define who is the connection connected to in the XML(routers-wise by router number and open a socket for them routers)
			//TODO: need to define the virtual IP's "connected" to the connection
			for (pugi::xml_node physicalRouterIter = iter.child(
					"VirtualConnection"); physicalRouterIter;
					physicalRouterIter = physicalRouterIter.next_sibling(
							"VirtualConnection"))
			{
				//TODO: next semester
			}
		}
	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
	}
}
void CEmulation::XMLParser(char * SetupFile)
{
	try
	{
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(SetupFile);
		if (!result)
		{
			throw(CException(
					"Error parsing file, make sure a *valid* XML file is present in the Emulation base folder\n"));
		}
		XMLVirtualConnectionsParser(doc);
		XMLRoutersParser(doc);

	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}

void CEmulation::StartEmulation()
{
	try
	{
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}

