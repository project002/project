/*
 * CVirtualConnection.cpp
 *
 *  Created on: Dec 31, 2013
 *      Author: root
 */

#include "CVirtualConnection.h"
#define FIRST_ROUTER_PACKET_COLLECTOR 0
#define SECOND_ROUTER_PACKET_COLLECTOR 1
//Byte Max Value
#define BM 255
#define UNIQUE_IP_FOR_VIRTUAL_CONNECTION "200.200.200.200"

CVirtualConnection::CVirtualConnection():mUniqueIPForConnection(new CUIPV4(UNIQUE_IP_FOR_VIRTUAL_CONNECTION))
{
	try
	{
		static int unsigned VC_INC=0;
		VC_INC += 1;
		id = VC_INC;
		mMacAddress = makeSequentialMACAddress();
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

CVirtualConnection::~CVirtualConnection()
{
	delete mUniqueIPForConnection;
}

string CVirtualConnection::makeSequentialMACAddress()
{
	int _id = (int)id; //probably won't go over INT_MAX
	stringstream ss;
	char temp[3] = {0};
	for (int i=5*BM;i>=0;i-=BM)
	{
		if ((_id-i) > 0)
		{
			sprintf(temp,"%02X",((_id-i) > 255 ? 255 : (_id-i)));
			ss << temp;
		}
		else
		{ss << "00";}
		if (i>0) {ss << ":";}
		//_id -= 255;
	}
	string mac = ss.str();
	return mac;
}

void CVirtualConnection::AddRoutingTableReference(map<string,pair<CConnection const*,string> > * routTable, int routerNumber)
{
	try
	{
		if (routerNumber == NO_CHOICE_OF_PACKET_COLLECTOR)
		{
			throw(CException(
					"Virtual Connection : Need packet collector number to withdraw from!"));
		}
		if (mRouterToPacketCollectorMap.find(routerNumber)
				== mRouterToPacketCollectorMap.end())
		{
			throw(CException(
					"Trying to get packet collector for invalid router"));
		}
		unsigned int tableOf = mRouterToPacketCollectorMap.at(
						routerNumber);
		mRoutingTable[tableOf]= routTable;
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

vector<pair<string, string> >& CVirtualConnection::GetTable()
{

	mMtx.lock();
	mRoutingToReturn.clear();
	try
	{
		map<string,pair<CConnection const*,string> >::iterator it=mRoutingTable[FIRST_ROUTER_PACKET_COLLECTOR]->begin();
		for (;it!=mRoutingTable[FIRST_ROUTER_PACKET_COLLECTOR]->end();it++)
		{
			mCombinedRoutingTable.insert((*it).first);
		}
		it=mRoutingTable[SECOND_ROUTER_PACKET_COLLECTOR]->begin();
		for (;it!=mRoutingTable[SECOND_ROUTER_PACKET_COLLECTOR]->end();it++)
		{
			mCombinedRoutingTable.insert((*it).first);
		}
		set< string >::iterator iterSet=mCombinedRoutingTable.begin();
		for (;iterSet!=mCombinedRoutingTable.end();iterSet++)
		{
			mRoutingToReturn.push_back(pair<string,string>((*iterSet),mMacAddress));
		}

		mMtx.unlock();
		return mRoutingToReturn;

	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
	mMtx.unlock();
	return mRoutingToReturn;
}

Crafter::Packet* CVirtualConnection::GetPacket(int routerNumber)
{
	try
	{
		double popTime;
		if (routerNumber == NO_CHOICE_OF_PACKET_COLLECTOR)
		{
			throw(CException(
					"Virtual Connection : Need packet collector number to withdraw from!"));
		}
		if (mRouterToPacketCollectorMap.find(routerNumber)
				== mRouterToPacketCollectorMap.end())
		{
			throw(CException(
					"Trying to get packet collector for invalid router"));
		}
		unsigned int fromCollector = mRouterToPacketCollectorMap.at(
				routerNumber);

		Crafter::Packet * pkt =mPacketCollectors[fromCollector].PopFront(popTime);

		return pkt;
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}

	return NULL;
}

void CVirtualConnection::SendPacket(Packet* packet,int routerNumber)
{
	try
	{
		if (packet!=NULL)
		{
			if (routerNumber==NO_CHOICE_OF_PACKET_COLLECTOR)
			{
				throw (CException("Virtual Connection : Need packet collector number to withdraw from!"));
			}
			if (mRouterToPacketCollectorMap.find(routerNumber)==mRouterToPacketCollectorMap.end())
			{
				throw (CException("Trying to get packet collector for invalid router"));
			}
			// send from router a to router b using xor on the packet collector related to the router
			unsigned int toSend=mRouterToPacketCollectorMap.at(routerNumber) ^ 1;

			Crafter::Packet * pkt=new Packet(*packet);

//			int id = getPacketIdentification(pkt);
//			if (id != -1) {SLogger::getInstance().Logf("::IN:: %d IP Packet ID %d",routerNumber,id);}

			mPacketCollectors[toSend].PushBack(pkt);

		}
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);

		throw;
	}
}

void CVirtualConnection::AddInvolvedRouter(const unsigned int & routerNumber)
{
	try
	{
		int packetCollectorNum = mInvolvedRoutersByNumber.size();
		mMtx.lock();
		mInvolvedRoutersByNumber.push_back(routerNumber);
		mRouterToPacketCollectorMap.insert(
				pair<unsigned int, unsigned int>(routerNumber,
						packetCollectorNum));
		mMtx.unlock();
		if (mInvolvedRoutersByNumber.size() > NUMBER_OF_CONNECTED_DEVICES)
		{
			throw(CException(
					"Virtual connection holding more than 2 connections!"));
		}
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		mMtx.unlock();
		throw;
	}
}
