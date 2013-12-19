/*
 * CPhysicalConnection.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#include "CPhysicalConnection.h"

CPhysicalConnection::CPhysicalConnection(struct ifaddrs* device):mMacAddress(NULL),mPacketCollector(NULL),mInterfaceName(NULL)
{
	try
	{
		// Init the structs with 0's
		InitStructs(device);

		// Configuring the socket
		ConfigureSocket(device);

		// Get the interface information
		GetInterfaceInformation();

		//TODO: Enable receiving of packets by calling ReceivePackets from CPhysicalConnection using a Thread!
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
		close(mSocket);
	}

}
/**
 * Initialization level function
 * The function sends an reversed ARP packet in order to retrieve all connected devices IP addresses
 */
void CPhysicalConnection::GetConnectedDevicesIPAddresses()
{
	try
	{
		//TODO: send arp request to all available ip addresses of subnet and get the active connected computers
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

void CPhysicalConnection::SetNetmask(int8_t maxNumberOfComputersInNetwork)
{
	try
	{
		struct ifreq ifr;
		memset(&ifr,0,sizeof(struct ifreq));
		strncpy(ifr.ifr_ifrn.ifrn_name,mInterfaceName,IFNAMSIZ);
		ifr.ifr_ifru.ifru_netmask.sa_family = AF_INET;
		ifr.ifr_ifru.ifru_addr.sa_family = AF_INET;

		// Setting maximum number of computers to be a power of 2 for the masking
		maxNumberOfComputersInNetwork--;
		maxNumberOfComputersInNetwork|=maxNumberOfComputersInNetwork >>1;
		maxNumberOfComputersInNetwork|=maxNumberOfComputersInNetwork >>2;
		maxNumberOfComputersInNetwork|=maxNumberOfComputersInNetwork >>4;
		maxNumberOfComputersInNetwork|=maxNumberOfComputersInNetwork >>8;
		maxNumberOfComputersInNetwork|=maxNumberOfComputersInNetwork >>16;
		maxNumberOfComputersInNetwork++;

		ifr.ifr_ifru.ifru_addr.sa_data[2]=rand()%255;
		ifr.ifr_ifru.ifru_addr.sa_data[3]=rand()%255;
		ifr.ifr_ifru.ifru_addr.sa_data[4]=rand()%255;
		ifr.ifr_ifru.ifru_addr.sa_data[5]=rand()%255;

		mIPAddress= new CIPv4Address(ifr.ifr_ifru.ifru_addr.sa_data,2);
		if (ioctl(mSocket, SIOCSIFADDR, &ifr) < 0)
		{
			perror("SIOCSIFNETMASK");
			throw(CException("Can't set a new IP"));
		}

		ifr.ifr_ifru.ifru_netmask.sa_data[2]=0xff;
		ifr.ifr_ifru.ifru_netmask.sa_data[3]=0xff;
		ifr.ifr_ifru.ifru_netmask.sa_data[4]=0xff;
		ifr.ifr_ifru.ifru_netmask.sa_data[5]=0xff-maxNumberOfComputersInNetwork+1;


		if (ioctl(mSocket, SIOCSIFNETMASK, &ifr) < 0)
		{
			perror("SIOCSIFNETMASK");
			throw(CException("Can't set a new net mask - Check if cable is connected"));
		}

		if(ioctl(mSocket,SIOCGIFFLAGS,&ifr)<0)
		{
			perror("SIOCGIFFLAGS");
			throw(CException("Can't get network flags."));
		}
		ifr.ifr_ifru.ifru_flags |= (IFF_UP | IFF_RUNNING);
		if(ioctl(mSocket,SIOCSIFFLAGS,&ifr)<0)
		{
			perror("SIOCSIFFLAGS");
			throw(CException("Can't set network flags."));
		}

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
		// setsockopt won't work on AF_PACKET family!
		/*if (setsockopt(mSocket, SOL_SOCKET, SO_BINDTODEVICE,
				(void *) &mIfreq, sizeof(mIfreq)) < 0)
		{
			throw CException(
					"Failed to add binding to specific interface flag");
		}*/
		struct ifreq ifr;
		memset(&ifr, 0, sizeof(struct ifreq));
		strncpy(ifr.ifr_ifrn.ifrn_name, mInterfaceName, IFNAMSIZ);
		if (ioctl(mSocket, SIOCGIFINDEX, &ifr) < 0)
		{
			perror("SIOCGIFINDEX");
			throw(CException("Can't get index."));
		}
		mInterfaceIndex= ifr.ifr_ifru.ifru_ivalue;

		sockaddr_ll addr= { 0 };
		addr.sll_family=AF_PACKET;
		addr.sll_ifindex=mInterfaceIndex;
		if(bind(mSocket,(sockaddr*)&addr,sizeof(sockaddr_ll))<0)
		{
			perror("Bind");
			throw(CException("Can't bind socket to interface using its index"));
		}

		mPacketCollector= new CPacketCollector(mSocket);
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
		mInterfaceName = new char [strlen(device->ifa_name)];
		strcpy(mInterfaceName,device->ifa_name);
	}
	catch(CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}
CPhysicalConnection::~CPhysicalConnection()
{
	close(mSocket);
	if(mPacketCollector!=NULL)
	{
		delete mPacketCollector;
		mPacketCollector=NULL;
	}
	if(mMacAddress!=NULL)
	{
		delete mMacAddress;
		mMacAddress=NULL;
	}
	if (mPacketCollector!=NULL)
	{
		delete mPacketCollector;
		mPacketCollector=NULL;
	}
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

//
//void CPhysicalConnection::Receive(int unsigned period_len)
//{
//	time_t start = time(NULL);
//	int len = (int) period_len;
//	char buffer[ETH_FRAME_LEN]={0};
//	ssize_t recvSize;
//	char* ipHead = &buffer[ETH_HLEN]; //after the Ethernet header
//
//	CMacAddress dMAC(buffer,0);
//	CMacAddress sMAC(buffer,ETH_ALEN);
//
//	while (time(NULL) < start+len)
//	{
//		try
//		{
//			recvSize=recvfrom(mSocket,buffer,ETH_FRAME_LEN,0,NULL,NULL);
//			if (recvSize == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
//			{
//				cerr << "err number " << errno << endl;
//				throw CException("fatal error on receive from socket");
//			}
//
//			if (!IsPacketEmpty(buffer)) //don't print empty packets
//			{
//				cout << "packet: " << endl;
//				cout << "\t Dest MAC: ";
//				//for (int dm=0;dm<ETH_ALEN;++dm) {printf("%02X:",(uint8_t)buffer[dm]);}
//				dMAC.SetMAC(buffer,0);
//				dMAC.Print();
//				cout << endl;
//
//				cout << "\t Src MAC:";
//				//for (int sm=0;sm<ETH_ALEN;++sm) {printf("%02X:",(uint8_t)buffer[ETH_ALEN+sm]);}
//				sMAC.SetMAC(buffer,ETH_ALEN);
//				sMAC.Print();
//				cout << endl;
//
//				cout << "\t Type ID:";
//				int unsigned typeID = 0;
//				typeID = typeID | buffer[ETH_ALEN*2];
//				typeID = typeID << 8;
//				typeID = typeID | buffer[(ETH_ALEN*2)+1];
//
//				printf("%04X",typeID);
//
//				switch (typeID)
//				{
//					case IPV4ID : cout << "(IPv4)";
//								//read_ipv4(ipHead);
//								break;
//					case IPV6ID : cout << "(IPv6)"; break;
//					case ARPID : cout << "(ARP)"; break;
//					case IPXID : cout << "(IPX)"; break;
//				}
//
//				cout << endl;
//
//				cout << "\t RAW: ";
//				for (int i=0;i<ETH_FRAME_LEN;++i)
//				{
//					printf("%02X",(uint8_t)buffer[i]);
//					buffer[i] = 0; //re-initialize the buffer
//				}
//				cout << endl;
//			}
//		}
//		catch (CException & e)
//		{
//			std::cerr << e.what() << std::endl;
//			perror("socket");
//			break;
//		}
//	}
//}

