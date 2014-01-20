/*
 * CPacketCollector.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: root
 */

#include "CPacketCollector.h"
//Defining the map
map<string,unsigned long long int> CPacketCollector::mPacketsStatistics;

CPacketCollector::CPacketCollector(unsigned int bufferSize):mBufferSize(bufferSize)
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

CPacketCollector::~CPacketCollector()
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

void CPacketCollector::PushBack(Crafter::Packet * pkt)
{
	try
	{
		//cout<< "[@]1[@]\n";
		if (mPackets.size()<mBufferSize)
		{
			//cout<< "[@]2[@]\n";
			mPackets.push_back(pkt);
			cout << mPackets.size() << endl;
			//AnalyzePacketForStatistics(pkt);
			//cout<< "[@]4[@]\n";
		}
		else
		{
			cout << "packet discarded" << endl;
			if (pkt!=NULL)
			{
				delete (pkt);
				pkt = NULL;
			}
		}
		//cout<< "[@]6[@]\n";
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}
void CPacketCollector::AnalyzePacketForStatistics(Crafter::Packet * packet)
{
	try
	{
		ARP* arp_layer = packet->GetLayer<ARP>();
		if (arp_layer != NULL)
		{
			mPacketsStatistics["ARP"]=mPacketsStatistics["ARP"]+1;
		}
		else
		{
			IP* ip_layer = packet->GetLayer<IP>();
			if (ip_layer != NULL)
			{
				mPacketsStatistics["IPv4"]=mPacketsStatistics["IPv4"]+1;
				TCP* tcp_layer = packet->GetLayer<TCP>();
				if(tcp_layer!=NULL)
				{
					mPacketsStatistics["TCP"]=mPacketsStatistics["TCP"]+1;
				}
				else
				{
					UDP* udp_layer = packet->GetLayer<UDP>();
					if (udp_layer!=NULL)
					{
						mPacketsStatistics["UDP"]=mPacketsStatistics["UDP"]+1;
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
Crafter::Packet * CPacketCollector::PopFront()
{
	try
	{
		if (!mPackets.empty())
		{
			Crafter::Packet * toret = mPackets.front();
			mPackets.pop_front();
			return toret;
		}
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
	return NULL;
}

void CPacketCollector::DropRandomPacket()
{
	try
	{
		if (!mPackets.empty())
		{
			list<Crafter::Packet*>::iterator iter = mPackets.begin();
			unsigned int cellNumber=rand()% mPackets.size();
			for (unsigned int i=0;i<cellNumber-1;i++,iter++);
			if ((*iter)!=NULL)
			{
				delete (*iter);
				(*iter)=NULL;
			}
			mPackets.erase(iter);
		}
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}

/**
 * Function for debugging. It prints the whole packet as it was received.
 * @param buffer pointer to the array's beginning
 * @param recvSize size of the received packet in bytes
 */
void CPacketCollector::PrintPacket()
{
	try
	{
		mPackets.front()->Print();
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		throw;
	}
}





















/* // If we want to implement more packet types there is a need for a switch :
CPacket * CPacketCollector::CreateIPv4Packet(char * buffer, ssize_t recvSize)
{

	try
	{
		uint8_t IPv4Type = buffer[ETH_HLEN +9];

		switch (IPv4Type)
		{
			case (IPPROTO_TCP):
					//TCP packet

				break;
			case (IPPROTO_ICMP):
					//ICMP packet

				break;
			case (IPPROTO_UDP):
					//UDP packet

				break;
			case (IPPROTO_RAW):
					//RAW IP packet - no data

				break;
			default:
				PrintPacket(buffer, recvSize);
				cerr  << endl;
				throw(CException("Can't find ethernet type"));
				break;
		}
		//TODO :switch with ipv4 protocols
		return NULL;
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}*/
