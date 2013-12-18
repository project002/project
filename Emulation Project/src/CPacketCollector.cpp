/*
 * CPacketCollector.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: root
 */

#include "CPacketCollector.h"

CPacketCollector::CPacketCollector(const int Socket):mSocket(Socket)
{
}

CPacketCollector::~CPacketCollector()
{
	try
	{
		list<CPacket * >::iterator it=mPackets.begin();
		for (;it!=mPackets.end();it++)
		{
			delete (*it);
		}
		mPackets.clear();
	}
	catch(CException & e)
	{
		std::cerr << e.what() << std::endl;
	}

}

void CPacketCollector::ReceivePackets()
{
	try
	{
		char buffer[ETH_FRAME_LEN] = { 0 };
		ssize_t recvSize;
		char* ipHead = &buffer[ETH_HLEN]; //after the Ethernet header
		CPacket * NewPacket=NULL;
		while (1)
		{
			try
			{
				recvSize = recvfrom(mSocket, buffer, ETH_FRAME_LEN, 0, NULL,
						NULL);
				if (recvSize == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
				{
					cerr << "err number " << errno << endl;
					throw CException("fatal error on receive from socket");
				}

				if (recvSize>0) //don't print empty packets
				{
					//TODO : get type id and decide which packet to create;
					NewPacket= CreatePacket(buffer,recvSize);
					if (NewPacket!=NULL)
					{
						mPackets.push_back(CreatePacket(buffer,recvSize));
					}

				}
			}
			catch (CException & e)
			{
				std::cerr << e.what() << std::endl;
				break;
			}
		}
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}

CPacket * CPacketCollector::CreatePacket(char * buffer, ssize_t recvSize)
{

	try
	{
		unsigned int ethernetType = uint16_t(buffer[ETH_ALEN * 2]);
		switch (ethernetType)
		{
			case (ETH_P_IP):
				return (new CPacketIPv4(buffer, recvSize)); // TODO check which protocol inside ipv4
				break;
			case (ETH_P_ARP):

				break;
			case (ETH_P_IPX):

				break;
			case (ETH_P_IPV6):

				break;
			default:
				throw(CException("Can't find ethernet type"));
				break;
		}
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}

CPacket * CPacketCollector::CreateIPv4Packet(char * buffer, ssize_t recvSize)
{

	try
	{
		//TODO :switch with ipv4 protocols
		return NULL;
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}

CPacket * CPacketCollector::CreateIPv6Packet(char * buffer, ssize_t recvSize)
{

	try
	{
		//TODO :switch with ipv6 protocols
		return NULL;
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}

CPacket * CPacketCollector::CreateIPXPacket(char * buffer, ssize_t recvSize)
{

	try
	{
		//TODO :switch with ipx protocols
		return NULL;
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;

	}
}

CPacket * CPacketCollector::CreateARPPacket(char * buffer, ssize_t recvSize)
{

	try
	{
		return new CPacketARP(buffer,recvSize);
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}

