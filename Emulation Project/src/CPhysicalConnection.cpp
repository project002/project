/*
 * CPhysicalConnection.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#include "CPhysicalConnection.h"

CPhysicalConnection::CPhysicalConnection(struct ifaddrs* device)
{
	try
	{
		// Init the structs with 0's
		InitStructs(device);

		// Configuring the socket
		ConfigureSocket(device);

		// Get the interface information
		GetInterfaceInformation();
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
		close(mSocket);
	}

}
void CPhysicalConnection::GetInterfaceInformation()
{
	try
	{
		// Getting the interface index
		if (ioctl(mSocket, SIOGIFINDEX, &mIfreq) < 0)
		{
			throw CException("Failed to retrieve interface index");
		}
		mInterfaceIndex = mIfreq.ifr_ifru.ifru_ivalue;

		// Getting the mac address of the interface
		if (ioctl(mSocket, SIOCGIFHWADDR, &mIfreq) < 0)
		{
			throw CException("Failed to retrieve interface index");
		}

		mMacAddress= new CMacAddress(mIfreq.ifr_addr.sa_data);

	}
	catch(CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}
void CPhysicalConnection::ConfigureSocket(struct ifaddrs* device)
{
	try
	{
		// Opening the socket
		mSocket = socket(device->ifa_addr->sa_family, SOCK_RAW | SOCK_NONBLOCK,
				htons(ETH_P_ALL));
		if (mSocket == -1)
		{
			throw CException("socket is bad");
		}

		// Adding flag to the socket to work only on a specific interface
		if (setsockopt(mSocket, SOL_SOCKET, SO_BINDTODEVICE,
				(void *) &mIfreq, sizeof(mIfreq)) < 0)
		{
			throw CException(
					"Failed to add binding to specific interface flag");
		}
	}
	catch(CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}
void CPhysicalConnection::InitStructs(struct ifaddrs* device)
{
	try
	{
		memset(&mIfreq, 0, sizeof(mIfreq));
		strncpy(mIfreq.ifr_ifrn.ifrn_name,device->ifa_name, IF_NAMESIZE);
	}
	catch(CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}
CPhysicalConnection::~CPhysicalConnection()
{
	// TODO Auto-generated destructor stub
	close(mSocket);
}

bool CPhysicalConnection::IsPacketEmpty(char* buffer)
{
	int sum = 0;
	for (int i=0;i<ETH_FRAME_LEN;++i)
	{
		sum += buffer[i];
		if (sum>0) {return false;}
	}
	return true;
}

void CPhysicalConnection::Receive(int unsigned period_len)
{
	time_t start = time(NULL);
	int len = (int) period_len;
	char buffer[ETH_FRAME_LEN]={0};
	ssize_t recvSize;
	char* ipHead = &buffer[ETH_HLEN]; //after the Ethernet header
	while (time(NULL) < start+len)
	{
		try
		{
			recvSize=recvfrom(mSocket,buffer,ETH_FRAME_LEN,0,NULL,NULL);
			if (recvSize == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
			{
				cerr << "err number " << errno << endl;
				throw CException("fatal error on receive from socket");
			}

			if (!IsPacketEmpty(buffer)) //don't print empty packets
			{
				cout << "packet: " << endl;
				cout << "\t Dest MAC: ";
				for (int dm=0;dm<ETH_ALEN;++dm) {printf("%02X:",(uint8_t)buffer[dm]);}
				cout << endl;

				cout << "\t Src MAC:";
				for (int sm=0;sm<ETH_ALEN;++sm) {printf("%02X:",(uint8_t)buffer[ETH_ALEN+sm]);}
				cout << endl;

				cout << "\t Type ID:";
				int unsigned typeID = 0;
				typeID = typeID | buffer[ETH_ALEN*2];
				typeID = typeID << 8;
				typeID = typeID | buffer[(ETH_ALEN*2)+1];

				printf("%04X",typeID);

				switch (typeID)
				{
					case IPV4ID : cout << "(IPv4)";
								//read_ipv4(ipHead);
								break;
					case IPV6ID : cout << "(IPv6)"; break;
					case ARPID : cout << "(ARP)"; break;
					case IPXID : cout << "(IPX)"; break;
				}

				cout << endl;

				cout << "\t RAW: ";
				for (int i=0;i<ETH_FRAME_LEN;++i)
				{
					printf("%02X",(uint8_t)buffer[i]);
					buffer[i] = 0; //re-initialize the buffer
				}
				cout << endl;
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

