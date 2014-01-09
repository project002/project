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
			vector< pair<string,string> >& tables=(*iter)->GetTable();
			//print table
			vector< pair<string,string> >::iterator b = tables.begin();
			for(;b!=tables.end();++b)
			{
				cout << b->first << " | " << b->second << endl;
			}
			/////////////
			vector< pair<string,string> >::iterator it=tables.begin();
			for (;it!=tables.end();it++)
			{
				mRoutingTable.insert(pair<string,pair<CConnection const*,string> >(
						it->first,
						pair<CConnection const*,string>((*iter),it->second)
					));
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


bool CRouter::ProcessSendPakcet(Packet* packet)
{
	cout << "[#] before change" << endl;
	packet->Print();
	map<string,pair< CConnection const*,string> >::iterator table_pos;
	IP* ip_layer = packet->GetLayer<IP>();

	Ethernet eth_layer;

	table_pos = mRoutingTable.find(ip_layer->GetDestinationIP());

	string DestMAC = table_pos->second.second;
	string SrcMAC = table_pos->second.first->GetMAC();

	eth_layer.SetSourceMAC(SrcMAC);
	eth_layer.SetDestinationMAC(DestMAC);

	packet->PopLayer(); //remove the original Ethernet layer
	packet->PushLayer(eth_layer); //add the new layer

	cout << "[#] after change" << endl;
	packet->Print();

	ip_layer->SetTTL(ip_layer->GetTTL()-1);
	return (ip_layer->GetTTL() >= 1);

}

void CRouter::PacketHandler()
{
	Packet* packet;
	CConnection* send_connection;
	map<string,pair< CConnection const*,string> >::iterator pos;
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
						send_connection = (const_cast<CConnection*> (pos->second.first));
						string dest_ip = send_connection->getGetwayAddress()->getIpStr();
						if(!dest_ip.compare(pos->first))
						{
							//then the IP packet is meant to this router. ignore it?
							//TODO: what to do with non DHCP packets aimed at this router
							cout<<"[#] Packet meant for router and ignored " << dest_ip <<endl;
						}
						else
						{
							cout<<"[#] Packet will be sent to next hop " << endl;
							if (ProcessSendPakcet(packet))
							{send_connection->SendPacket(packet);}
						}
					}
				}
				else
				{
					ARP* arp_layer = packet->GetLayer<ARP>();
					if (arp_layer != NULL) //answer arp requests
					{
						cout<< "************************ Received Packet ************************\n";
						packet->Print();
						cout << "[#] handeling ARP" << endl;
						pos = mRoutingTable.find(arp_layer->GetTargetIP());

						if (pos!=mRoutingTable.end())
						{
							cout << "[#] handeling ARP: formulating replay" << endl;
							send_connection = (const_cast<CConnection*> (pos->second.first));

							arp_layer->SetTargetMAC(send_connection->GetMAC());

							//change for replay OpCode
							arp_layer->SetOperation(ARP::Reply);

							//Set target properties as Requester PC
							string senderIP = arp_layer->GetTargetIP();
							arp_layer->SetTargetIP(arp_layer->GetSenderIP());
							arp_layer->SetTargetMAC(arp_layer->GetSenderMAC());

							//Set sender properties as ME (Emulation)
							arp_layer->SetSenderIP(senderIP);
							arp_layer->SetSenderMAC(pos->second.second);

							//Setting ethernet layer properties
							Ethernet* eth_layer = packet->GetLayer<Ethernet>();

							string DestMAC = eth_layer->GetSourceMAC();
							string SrcMAC = pos->second.first->GetMAC();

							eth_layer->SetSourceMAC(SrcMAC);
							eth_layer->SetDestinationMAC(DestMAC);

//							packet->PopLayer(); //remove the original Ethernet layer
//							packet->PushLayer(eth_layer); //add the new layer

							cout << "[#] handeling ARP: sending packet" << endl;
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

