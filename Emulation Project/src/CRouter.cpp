/*
 * CRouter.cpp
 *
 *  Created on: Dec 31, 2013
 *      Author: root
 */

#include "CRouter.h"

CRouter::CRouter():mBufferSize(DEFAULT_ROUTER_BUFFER_SIZE),mPacketCollector(NULL)
{
	try
	{
	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}

}
void CRouter::RequestTables()
{
	try
	{
		list<CConnection const *>::iterator iter;
		//iterate over all connections
		for(iter = mConnections.begin();iter!=mConnections.end();iter++)
		{
			//iterate over all ips in the table you got from the connection
			vector<string>& tables=(*iter)->GetTable();
			vector<string>::iterator it=tables.begin();
			for (;it!=tables.end();it++)
			{
				mRoutingTable.insert(pair<string,CConnection const*>((*it),(*iter)));
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

void CRouter::Sniffer()
{
	try
	{
		mPacketCollector= new CPacketCollector(mBufferSize);
		mSniffingThread = boost::thread(&CRouter::Sniff, this);
		mPacketsHandlingThread = boost::thread(&CRouter::PacketHandler, this);
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}

void CRouter::PacketHandler()
{
	Packet* packet;
	CConnection* send_connection;
	map<string,CConnection const*>::iterator pos;
	try
	{
		//this function is in a new thread. TODO: make it handle all packets in packet handler
		while (true)
		{
			packet = mPacketCollector->PopFront();
			if (packet != NULL)
			{
				IP* ip_layer = packet->GetLayer<IP>();
				if (ip_layer != NULL)
				{
					pos  = mRoutingTable.find(ip_layer->GetDestinationIP());
					if (pos!=mRoutingTable.end())
					{
						send_connection = (const_cast<CConnection*> (pos->second));
						if(!send_connection->getGetwayAddress()->getIpStr().compare(pos->first))
						{
							//TODO:Handle packet as Router's mission
							cout<<"Packet meant for router\n";
						}
						else
						{
							cout<<"Packet will be sent to next hop\n";
							send_connection->SendPacket(packet);
						}
					}
				}
			}

		}
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}
void CRouter::Sniff()
{
	Packet* temp_packet;
	try
	{
		while(true)
		{
			list< CConnection const *>::iterator iter = mConnections.begin();
			for (;iter!=mConnections.end();iter++)
			{
				CConnection * connection = const_cast<CConnection*>(*iter);
				temp_packet= connection->GetPacket();
				if (temp_packet != NULL)
				{mPacketCollector->PushBack(temp_packet);}
			}
		}
		//this function is in a new thread. TODO: make it sniff all packets
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}
CRouter::~CRouter()
{
	try
	{
		mConnections.clear();
		if(mPacketCollector!=NULL)
		{
			delete mPacketCollector;
			mPacketCollector=NULL;
		}
	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}

