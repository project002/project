/*
 * CXMLBasicParser.h
 *
 *  Created on: Apr 8, 2014
 *      Author: root
 */

#ifndef CXMLBASICPARSER_H_
#define CXMLBASICPARSER_H_


#include "BasicIncludes.h"
#include "pugixml.hpp"
#include "SXMLStructs.h"
#include "DParserDefs.h"
#include <map>
#define ERROR_MSG_XML_PARSER "Error parsing file, make sure a *valid* XML file is present in the Emulation base folder\n"

class CXMLBasicParser
{
public:
	CXMLBasicParser(){}
	void ParseXML(string fileName)
	{
		OpenFile(fileName);
		ParseIntoMembers();
	}
	unsigned int GetNumberOfRouters()const{return mRoutersInformation.size();}
	vector<RouterInformation> GetRoutersInformation()const{return mRoutersInformation;}
	multimap<unsigned int,unsigned int> GetVirtualConnections()const{return mConnectionsBetweenRouters;}
	multimap<unsigned int,string> GetPhysicalConnections()const{return mPhysicalConnections;}
	unsigned int GetNumberOfConnectionPerRouter(unsigned int routerNumber)
	{
		int vConCount = getVirtualConnectioCount(routerNumber);
		return vConCount + mPhysicalConnections.count(routerNumber);
	}
	bool routerExists(unsigned int routerNumber)
	{
		vector<RouterInformation>::iterator it = mRoutersInformation.begin();
		for (;it!=mRoutersInformation.end();++it) {if ((*it).sRouterNumber == routerNumber) {return true;}}
		return false;
	}
	bool isPhysicalRouter(unsigned int routerNumber)
	{
		return (mPhysicalConnections.count(routerNumber) != 0);
	}
	pair<int, int> GetRouterPosition(unsigned int routerNumber)
	{
		if (mPositionMap.find(routerNumber)!=mPositionMap.end())
		{
			return mPositionMap[routerNumber];
		}
		return pair<int,int>(0,0);
	}
	bool isThreaded(){return doc.child(XML_LAYER_1_NETWORK).attribute(THREADED_EMULATION).as_bool(false);}
private:
	//map router number and its position
	map< unsigned int,pair <int,int> > mPositionMap;
	pugi::xml_document doc;
	// router num to router nums
	multimap<unsigned int,unsigned int> mConnectionsBetweenRouters;
	// router information struct
	vector<RouterInformation> mRoutersInformation;
	// router to physical connection
	multimap<unsigned int,string> mPhysicalConnections;

	void OpenFile(string fileName)
	{
		pugi::xml_parse_result result = doc.load_file(fileName.c_str());
		if (!result)
		{
			throw(CException(ERROR_MSG_XML_PARSER));
		};
	}

	void ParseIntoMembers()
	{
		ParseRoutersInformation();
		ParseVirtualConnections();
	}

	void ParsePhysicalConnections(pugi::xml_node iter)
	{
		for (pugi::xml_node physicalRouterIter = iter.child(
		XML_LAYER_4_INDIVIDUAL_ROUTERS_PHYSICAL_CONNECTION); physicalRouterIter;
				physicalRouterIter = physicalRouterIter.next_sibling(
				XML_LAYER_4_INDIVIDUAL_ROUTERS_PHYSICAL_CONNECTION))
		{
			unsigned int routerNumber = iter.attribute(
								XML_ROUTER_NUMBER_ATTRIBUTE).as_int();

			string PhysicalConnectionName = string(
					physicalRouterIter.child_value());
			mPhysicalConnections.insert(pair<unsigned int,string>(routerNumber,PhysicalConnectionName));
		}
	}

	void ParseRoutersInformation()
	{
		pugi::xml_node Routers = doc.child(XML_LAYER_1_NETWORK).child(
				XML_LAYER_2_ROUTERS);

		for (pugi::xml_node currentRouter = Routers.child(
				XML_LAYER_3_INDIVIDUAL_ROUTERS); currentRouter; currentRouter =
				currentRouter.next_sibling(XML_LAYER_3_INDIVIDUAL_ROUTERS))
		{
			RouterInformation sRouterInfo;
			ParsePhysicalConnections(currentRouter);
			sRouterInfo.sRouterNumber = currentRouter.attribute(
					XML_ROUTER_NUMBER_ATTRIBUTE).as_int();

			sRouterInfo.sBufferSize = currentRouter.attribute(
					XML_ROUTER_BUFFER_SIZE_ATTRIBUTE).as_int();

			sRouterInfo.sDropRate=currentRouter.attribute(
					XML_ROUTER_DROP_RATE_ATTRIBUTE).as_double();

			sRouterInfo.sUsedBufferSize = currentRouter.attribute(
					XML_ROUTER_INITIAL_USED_BUFFER_SIZE_ATTRIBUTE).as_int();

			sRouterInfo.sFillage = currentRouter.attribute(
					XML_ROUTER_FILLAGE_ATTRIBUTE).as_double();
			sRouterInfo.sDynamicFillage = currentRouter.attribute(
					XML_ROUTER_DYNAMIC_FILLAGE_ARRAY_ATTRIBUTE).as_string();
			sRouterInfo.sDynamicDropRate = currentRouter.attribute(
					XML_ROUTER_DYNAMIC_DROP_RATE_ARRAY_ATTRIBUTE).as_string();

			int xPos=currentRouter.attribute(
					XML_ROUTER_X_POS_ATTRIBUTE).as_int();
			int yPos=currentRouter.attribute(
					XML_ROUTER_Y_POS_ATTRIBUTE).as_int();
			mPositionMap[sRouterInfo.sRouterNumber]=pair<int,int>(xPos,yPos);
			mRoutersInformation.push_back(sRouterInfo);
		}
	}

	void ParseVirtualConnections()
	{
		pugi::xml_node VirtualConnections =
				doc.child(XML_LAYER_1_NETWORK).child(
						XML_LAYER_2_VIRTUAL_CONNECTIONS);

		for (pugi::xml_node iter = VirtualConnections.child(
				XML_LAYER_3_INDIVIDUAL_VIRTUAL_CONNECTIONS); iter;
				iter = iter.next_sibling(
						XML_LAYER_3_INDIVIDUAL_VIRTUAL_CONNECTIONS))
		{

			pugi::xml_node virtualConnectionIter = iter.child(
						XML_LAYER_4_INDIVIDUAL_VIRTUAL_CONNECTIONS_ROUTER_NUMBER);
			unsigned int first = virtualConnectionIter.attribute(
					XML_ROUTER_NUMBER_ATTRIBUTE).as_int();
			virtualConnectionIter = virtualConnectionIter.next_sibling(
								XML_LAYER_4_INDIVIDUAL_VIRTUAL_CONNECTIONS_ROUTER_NUMBER);
			unsigned int second = virtualConnectionIter.attribute(
					XML_ROUTER_NUMBER_ATTRIBUTE).as_int();

			mConnectionsBetweenRouters.insert(pair<unsigned int,unsigned int>(first,second));
		}
	}

	int getVirtualConnectioCount(unsigned int routerNumber)
	{
		int fromRouter = mConnectionsBetweenRouters.count(routerNumber);
		int toRouter = 0;
		multimap<unsigned int,unsigned int>::iterator it = mConnectionsBetweenRouters.begin();
		for (; it!=mConnectionsBetweenRouters.end();++it)
		{
			if ((*it).first == routerNumber) {continue;}
			if ((*it).second == routerNumber &&
			    routerExists((*it).first))
			{++toRouter;}
		}
		return fromRouter+toRouter;
	}
};

#endif /* CXMLBASICPARSER_H_ */
