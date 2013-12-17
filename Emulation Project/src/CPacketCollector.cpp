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
		time_t start = time(NULL);
		char buffer[ETH_FRAME_LEN] = { 0 };
		ssize_t recvSize;
		char* ipHead = &buffer[ETH_HLEN]; //after the Ethernet header

		CMacAddress dMAC(buffer, 0);
		CMacAddress sMAC(buffer, ETH_ALEN);
		CPacket * NewPacket;
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
				    NewPacket = new CPacket(buffer,recvSize);
					mPackets.push_back(NewPacket);
/*
					cout << "packet: " << endl;
					cout << "\t Dest MAC: ";
					//for (int dm=0;dm<ETH_ALEN;++dm) {printf("%02X:",(uint8_t)buffer[dm]);}
					dMAC.SetMAC(buffer, 0);
					dMAC.Print();
					cout << endl;

					cout << "\t Src MAC:";
					//for (int sm=0;sm<ETH_ALEN;++sm) {printf("%02X:",(uint8_t)buffer[ETH_ALEN+sm]);}
					sMAC.SetMAC(buffer, ETH_ALEN);
					sMAC.Print();
					cout << endl;

					cout << "\t Type ID:";
					int unsigned typeID = 0;
					typeID = typeID | buffer[ETH_ALEN * 2];
					typeID = typeID << 8;
					typeID = typeID | buffer[(ETH_ALEN * 2) + 1];

					printf("%04X", typeID);

					switch (typeID)
					{
						case IPV4ID:
							cout << "(IPv4)";
							//read_ipv4(ipHead);
							break;
						case IPV6ID:
							cout << "(IPv6)";
							break;
						case ARPID:
							cout << "(ARP)";
							break;
						case IPXID:
							cout << "(IPX)";
							break;
					}

					cout << endl;

					cout << "\t RAW: ";
					for (int i = 0; i < ETH_FRAME_LEN; ++i)
					{
						printf("%02X", (uint8_t) buffer[i]);
						buffer[i] = 0; //re-initialize the buffer
					}
					cout << endl;*/
				}
			}
			catch (CException & e)
			{
				std::cerr << e.what() << std::endl;
				perror("socket");
				break;
			}
		}
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}
