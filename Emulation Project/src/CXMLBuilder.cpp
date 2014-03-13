/*
 * CXMLBuilder.cpp
 *
 *  Created on: Mar 11, 2014
 *      Author: root
 */

#include "CXMLBuilder.h"
#include <fstream>
#include <boost/filesystem.hpp>
#include "DParserDefs.h"
#define DEFAULT_THREADED_EMULATION "true"
#define DEFAULT_FORMAT_VERSION 1
#define MAXIMUM_DROP_RATE_IN_PERCENTAGE 7
#define MINIMAL_BUFFER_SIZE_IN_PACKETS 0

CXMLBuilder::CXMLBuilder():mNoCurrentRouterChosen(true),mCurrentFileName("")
{
	try
	{
		GenerateNextFileName();
		InitXML();
	}
	catch(CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}
CXMLBuilder::CXMLBuilder(string fileName):mNoCurrentRouterChosen(true),mCurrentFileName(fileName)
{
	try
	{
		InitXML();
	}
	catch(CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}
void CXMLBuilder::InitXML()
{
	try
	{
		OpenFileForStream();
		BuildDefaultXMLInformation();
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}
CXMLBuilder::~CXMLBuilder()
{
	try
	{
	}
	catch(CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}
/**
 * Function opens a file names after the generated file name in the previous steps
 * The file is opened for output only and being truncated if it exists.
 */
void CXMLBuilder::OpenFileForStream()
{
	try
	{
		if(!mFile.is_open())
		{
			mFile.close();
		}
		mFile.open(mCurrentFileName.c_str(),std::ofstream::trunc | std::ofstream::out);
		if(!mFile.is_open())
		{
			throw(CException("File couldn't be opened!"));
		}
	}
	catch(CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

void CXMLBuilder::Finalize()
{
	try
	{
		//Reopen file so it is truncated
		OpenFileForStream();
		//Save new XML to file
		mDoc.save_file(mCurrentFileName.c_str());
	}
	catch(CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}
/**
 * Function checks whether a file exists or not.
 * It iterated over all Setup_#.xml files and finishes when a new file can be created without deleting an
 * existing XML.
 */
void CXMLBuilder::GenerateNextFileName()
{
	try
	{
		unsigned int fileNumber=0;
		while(true)
		{
			stringstream ss;
			ss.clear();
			ss<<"Setup_"<<fileNumber<<".xml";
		    boost::filesystem::path myfile(ss.str().c_str());
		    if( !boost::filesystem::exists(myfile) )
		    {
		        mCurrentFileName=ss.str();
		        break;
		    }
		    fileNumber++;
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
 * Function adds to the xml file the basic lines needed in the xml which includes
 * - Network
 * -- Routers
 * -- Virtual Connections
 */
void CXMLBuilder::BuildDefaultXMLInformation()
{
	try
	{
		//Adding Network layer
		mNetworkNode = mDoc.append_child(XML_LAYER_1_NETWORK);
		mNetworkNode.append_attribute(FORMAT_VERSION) = DEFAULT_FORMAT_VERSION;
		mNetworkNode.append_attribute(THREADED_EMULATION) =
				DEFAULT_THREADED_EMULATION;

		//Adding Routers layer
		mRoutersNode = mNetworkNode.append_child(
				XML_LAYER_2_ROUTERS);
		//Adding Virtual Connections layer
		mVirtualConnectionsNode = mNetworkNode.append_child(
				XML_LAYER_2_VIRTUAL_CONNECTIONS);

	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

/**
 * Adding a router to the XML file
 * @param routerInfo holding router number buffer size and drop rate wanted
 * 		  for the current router
 * @return false if can't be added(wrong values entered or duplicate router number was found)
 */
bool CXMLBuilder::AddRouter(RouterInformation routerInfo)
{
	try
	{
		//Verify the router info correctness and that there are no duplication of routers
		if (!CheckRouterCorrectness(routerInfo))
		{
			return false;
		}
		//Appending a router
		mCurrentRouter = mRoutersNode.append_child(
				XML_LAYER_3_INDIVIDUAL_ROUTERS);

		mNoCurrentRouterChosen=false;
		//Appending attributes of router number buffer size and drop rate
		//according to the information passed to the function
		mCurrentRouter.append_attribute(XML_ROUTER_NUMBER_ATTRIBUTE) =
				routerInfo.sRouterNumber;
		mCurrentRouter.append_attribute(XML_ROUTER_BUFFER_SIZE_ATTRIBUTE) =
				routerInfo.sBufferSize;
		mCurrentRouter.append_attribute(XML_ROUTER_DROP_RATE_ATTRIBUTE) =
				routerInfo.sDropRate;
		return true;
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

/**
 * Function adds a physical connection to the last added router or
 * the last edited router.
 * @param pcName interface name
 * @return true if was added succesfuly
 */
bool CXMLBuilder::AddPhysicalConnection(string pcName)
{
	try
	{
		if(!CheckPhysicalConnectionCorrectness(pcName) || mNoCurrentRouterChosen)
		{
			return false;
		}

		pugi::xml_node pcNode = mCurrentRouter.append_child(
				XML_LAYER_4_INDIVIDUAL_ROUTERS_PHYSICAL_CONNECTION);
		pcNode.append_child(pugi::node_pcdata).set_value(pcName.c_str());

		return true;
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

/**
 * Function checks if the interface name provided is not already entered in current router
 * @param pcName
 * @return false if interface name is already present
 */
bool CXMLBuilder::CheckPhysicalConnectionCorrectness(string pcName)
{
	try
	{
		for (pugi::xml_node physicalCon = mCurrentRouter.child(
		XML_LAYER_4_INDIVIDUAL_ROUTERS_PHYSICAL_CONNECTION); physicalCon;
				physicalCon = physicalCon.next_sibling(
				XML_LAYER_4_INDIVIDUAL_ROUTERS_PHYSICAL_CONNECTION))
		{
			string interfaceName = physicalCon.child_value();
			if (!interfaceName.compare(pcName))
			{
				return false;
			}
		}
		return true;
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}
/**
 * Checks the router information for illegal values or duplication of routers
 * in the XML file
 * @param routerInfo
 * @return false if bad buffersize droprate or duplication of routers
 */
bool CXMLBuilder::CheckRouterCorrectness(RouterInformation routerInfo)
{
	try
	{
		if (routerInfo.sBufferSize<MINIMAL_BUFFER_SIZE_IN_PACKETS)
		{
			return false;
		}
		if (routerInfo.sDropRate>MAXIMUM_DROP_RATE_IN_PERCENTAGE)
		{
			return false;
		}
		if (!IsRouterNumberFree(routerInfo.sRouterNumber))
		{
			return false;
		}
		return true;
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

/**
 * Verify that the router number given is not a duplication
 * @param newRouterNumber
 * @return false if there is a duplication
 */
bool CXMLBuilder::IsRouterNumberFree(unsigned int newRouterNumber)
{
	try
	{
		for (pugi::xml_node currentRouter = mRoutersNode.child(
				XML_LAYER_3_INDIVIDUAL_ROUTERS); currentRouter; currentRouter =
				currentRouter.next_sibling(XML_LAYER_3_INDIVIDUAL_ROUTERS))
		{
			unsigned int RouterNumber = currentRouter.attribute(XML_ROUTER_NUMBER_ATTRIBUTE).as_int();
			if(RouterNumber==newRouterNumber)
			{
				return false;
			}
		}
		return true;
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

/**
 * Function checks if a router exists and removes it
 * @param routerNumber router number to remove
 * @return true if removed succesfuly
 */
bool CXMLBuilder::RemoveRouter(unsigned int routerNumber)
{
	try
	{
		if(!SetCurrentRouter(routerNumber))
		{
			return false;
		}
		mRoutersNode.remove_child(mCurrentRouter);
		mNoCurrentRouterChosen=true;
		return true;
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

/**
 * Function checks if a PC exists and removes it
 * @param pcName
 * @return true if removed succesfuly
 */
bool CXMLBuilder::RemovePhysicalConnection(string pcName)
{
	try
	{
		if (mNoCurrentRouterChosen)
		{
			return false;
		}
		for (pugi::xml_node physicalCon = mCurrentRouter.child(
		XML_LAYER_4_INDIVIDUAL_ROUTERS_PHYSICAL_CONNECTION); physicalCon;
				physicalCon = physicalCon.next_sibling(
				XML_LAYER_4_INDIVIDUAL_ROUTERS_PHYSICAL_CONNECTION))
		{
			string interfaceName = physicalCon.child_value();
			if (!interfaceName.compare(pcName))
			{
				mCurrentRouter.remove_child(physicalCon);
			}
		}
		return true;
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

/**
 * function selects the current router to work on
 * @param routerNumber
 * @return true if could set active router or false if it doesnt exist
 */
bool CXMLBuilder::SetCurrentRouter(unsigned int routerNumber)
{
	try
	{
		//If router number if free meaning no such router and nothing to set as current
		if(IsRouterNumberFree(routerNumber))
		{
			return false;
		}
		for (pugi::xml_node currentRouter = mRoutersNode.child(
		XML_LAYER_3_INDIVIDUAL_ROUTERS); currentRouter; currentRouter =
				currentRouter.next_sibling(XML_LAYER_3_INDIVIDUAL_ROUTERS))
		{
			unsigned int RouterNumber = currentRouter.attribute(
					XML_ROUTER_NUMBER_ATTRIBUTE).as_int();
			if (RouterNumber == routerNumber)
			{
				mCurrentRouter=currentRouter;
				mNoCurrentRouterChosen=false;
				break;
			}
		}
		return true;
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}
/**
 * Function returns the chosen router information in case we want to display
 * the current router info when the user chooses to edit it.
 * @return current router information
 */
RouterInformation CXMLBuilder::GetCurrentRouterInformation()
{
	try
	{
		RouterInformation rtInfo;
		rtInfo.sBufferSize=0;rtInfo.sDropRate=0;rtInfo.sRouterNumber=0;

		rtInfo.sRouterNumber = mCurrentRouter.attribute(XML_ROUTER_NUMBER_ATTRIBUTE).as_int();
		rtInfo.sBufferSize = mCurrentRouter.attribute(XML_ROUTER_BUFFER_SIZE_ATTRIBUTE).as_int();
		rtInfo.sDropRate = mCurrentRouter.attribute(XML_ROUTER_DROP_RATE_ATTRIBUTE).as_int();

		return rtInfo;
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}
bool CXMLBuilder::EditCurrentRouterInformation(RouterInformation routerInfo)
{
	try
	{
		if (routerInfo.sBufferSize < MINIMAL_BUFFER_SIZE_IN_PACKETS)
		{
			return false;
		}
		if (routerInfo.sDropRate > MAXIMUM_DROP_RATE_IN_PERCENTAGE)
		{
			return false;
		}
		if( routerInfo.sRouterNumber == mCurrentRouter.attribute(XML_ROUTER_NUMBER_ATTRIBUTE).as_uint() ||
				IsRouterNumberFree(routerInfo.sRouterNumber))
		{
			//Appending attributes of router number buffer size and drop rate
			//according to the information passed to the function
			mCurrentRouter.attribute(XML_ROUTER_NUMBER_ATTRIBUTE) =
					routerInfo.sRouterNumber;
			mCurrentRouter.attribute(XML_ROUTER_BUFFER_SIZE_ATTRIBUTE) =
					routerInfo.sBufferSize;
			mCurrentRouter.attribute(XML_ROUTER_DROP_RATE_ATTRIBUTE) =
					routerInfo.sDropRate;
			return true;
		}
		else
		{
			return false;
		}
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}


bool CXMLBuilder::AddVirtualConnection(unsigned int firstRouter,
		unsigned int secondRouter)
{
	try
	{
		if (firstRouter == secondRouter || IsRouterNumberFree(firstRouter)
				|| IsRouterNumberFree(secondRouter) || IsVirtualConnectionExist(firstRouter,secondRouter))
		{
			return false;
		}
		stringstream s1;
		s1<<firstRouter;
		stringstream s2;
		s2<<secondRouter;
		//Appending a router
		pugi::xml_node virtualCon = mVirtualConnectionsNode.append_child(
				XML_LAYER_3_INDIVIDUAL_VIRTUAL_CONNECTIONS);

		pugi::xml_node firstRouter = virtualCon.append_child(
				XML_LAYER_4_INDIVIDUAL_VIRTUAL_CONNECTIONS_ROUTER_NUMBER);
		firstRouter.append_child(pugi::node_pcdata).set_value(s1.str().c_str());
		pugi::xml_node secondRouter = virtualCon.append_child(
				XML_LAYER_4_INDIVIDUAL_VIRTUAL_CONNECTIONS_ROUTER_NUMBER);
		secondRouter.append_child(pugi::node_pcdata).set_value(s2.str().c_str());

		return true;
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

bool CXMLBuilder::IsVirtualConnectionExist(unsigned int firstRouter,
		unsigned int secondRouter)
{
	try
	{
		stringstream s1;
		stringstream s2;
		s1<<firstRouter;
		s2<<secondRouter;
		bool first=false;
		bool second=false;
		for (pugi::xml_node currentVirtualCon = mVirtualConnectionsNode.child(
		XML_LAYER_3_INDIVIDUAL_VIRTUAL_CONNECTIONS); currentVirtualCon;
				currentVirtualCon = currentVirtualCon.next_sibling(
				XML_LAYER_3_INDIVIDUAL_VIRTUAL_CONNECTIONS))
		{
			for (pugi::xml_node currentVirtualVal = currentVirtualCon.child(
			XML_LAYER_4_INDIVIDUAL_VIRTUAL_CONNECTIONS_ROUTER_NUMBER);
					currentVirtualVal;
					currentVirtualVal = currentVirtualVal.next_sibling(
					XML_LAYER_4_INDIVIDUAL_VIRTUAL_CONNECTIONS_ROUTER_NUMBER))
			{
				if(!s1.str().compare(currentVirtualVal.child_value()))
				{
					first=true;
				}
				if(!s2.str().compare(currentVirtualVal.child_value()))
				{
					second=true;
				}
			}
			if (first && second)
			{
				return true;
			}
			first=false;
			second=false;
		}
		return false;
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

bool CXMLBuilder::RemoveVirtualConnection(unsigned int firstRouter,
		unsigned int secondRouter)
{
	try
	{
		bool first=false;
		bool second=false;
		stringstream s1;
		stringstream s2;
		s1<<firstRouter;
		s2<<secondRouter;
		for (pugi::xml_node currentVirtualCon = mVirtualConnectionsNode.child(
		XML_LAYER_3_INDIVIDUAL_VIRTUAL_CONNECTIONS); currentVirtualCon;
				currentVirtualCon = currentVirtualCon.next_sibling(
						XML_LAYER_3_INDIVIDUAL_VIRTUAL_CONNECTIONS))
		{
			for (pugi::xml_node currentVirtualCon =
					mVirtualConnectionsNode.child(
					XML_LAYER_3_INDIVIDUAL_VIRTUAL_CONNECTIONS);
					currentVirtualCon;
					currentVirtualCon = currentVirtualCon.next_sibling(
					XML_LAYER_3_INDIVIDUAL_VIRTUAL_CONNECTIONS))
			{
				for (pugi::xml_node currentVirtualVal = currentVirtualCon.child(
				XML_LAYER_4_INDIVIDUAL_VIRTUAL_CONNECTIONS_ROUTER_NUMBER);
						currentVirtualVal;
						currentVirtualVal =
								currentVirtualVal.next_sibling(
										XML_LAYER_4_INDIVIDUAL_VIRTUAL_CONNECTIONS_ROUTER_NUMBER))
				{
					if (!s1.str().compare(currentVirtualVal.value()))
					{
						first = true;
					}
					if (!s2.str().compare(currentVirtualVal.value()))
					{
						second = true;
					}
				}
				if (first && second)
				{
					mVirtualConnectionsNode.remove_child(currentVirtualCon);
					break;
				}
				first=false;
				second=false;
			}
		}
		return true;
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}
