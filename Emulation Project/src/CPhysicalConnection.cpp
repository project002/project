/*
 * CPhysicalConnection.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#include "CPhysicalConnection.h"

CPhysicalConnection::CPhysicalConnection(struct ifaddrs* device):
	mSocket(-1),mPacketCollector(NULL),mInterfaceName(NULL)
{

	try
	{
		// Init the structs with 0's
		InitStructs(device);

		// Configuring the socket
		ConfigureSocket(device);

		// Get the interface information
		GetInterfaceInformation();

		//Set the Subnet Mask
		setMaskAddress();

		mDHCPsrv = new CDHCPService(mInterfaceName,mIPMaskAddress->getIpArr());
		const char* getway_addr = mDHCPsrv->getIPAddr();
		mGetwayAddress = new CUIPV4(string(getway_addr));

		//TODO: Try to avoid sending packets
		SetNetmask(mGetwayAddress,mIPMaskAddress);

		startDHCPService();

	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		close(mSocket);
	}

}

void CPhysicalConnection::startDHCPService()
{
//	string iFace(mInterfaceName);
//	CDHCPPacketHandler::mInterfaceName = mInterfaceName;
//	CDHCPPacketHandler::mIPMaskAddress = mIPMaskAddress->getIpArr();
//	CDHCPPacketHandler::mIPAddressSet = "10.0.0.2";
//	Sniffer sniff("udp and src port 68 and dst port 67",iFace,CDHCPPacketHandler::startDHCPhandshake);

//	sniff.Capture();

	cout << "DHCP service started..." << endl;
	mDHCPSniffer = new Sniffer(CDHCPService::DHCP_FILTER,string(mInterfaceName),runDHCPService);

	mDHCPSniffer->Capture(-1,static_cast<void*> (mDHCPsrv));
}


void CPhysicalConnection::setMaskAddress()
{
	stringstream ss;
	uint8_t subnet[] = {0xff,0xff,0xff,0x00};
	mIPMaskAddress = new CUIPV4(subnet);
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


	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		close(mSocket);
	}
}

void CPhysicalConnection::SetNetmask(CUIPV4* getway_addr,CUIPV4* mIPMaskAddress)
{
	try
	{
		stringstream ss;
		struct ifreq ifr;
		memset(&ifr,0,sizeof(struct ifreq));
		strncpy(ifr.ifr_ifrn.ifrn_name,mInterfaceName,IFNAMSIZ);
		ifr.ifr_ifru.ifru_netmask.sa_family = AF_INET;
		ifr.ifr_ifru.ifru_addr.sa_family = AF_INET;

		const uint8_t* addr = getway_addr->getIpArr();

		// Setting random IP address for the connection
		ifr.ifr_ifru.ifru_addr.sa_data[2]=addr[0];
		ifr.ifr_ifru.ifru_addr.sa_data[3]=addr[1];
		ifr.ifr_ifru.ifru_addr.sa_data[4]=addr[2];
		ifr.ifr_ifru.ifru_addr.sa_data[5]=addr[3];

		if (ioctl(mSocket, SIOCSIFADDR, &ifr) < 0)
		{
			perror("SIOCSIFNETMASK");
			throw(CException("Can't set a new IP"));
		}
		ss.clear();

		const uint8_t* mask = mIPMaskAddress->getIpArr();

		ifr.ifr_ifru.ifru_netmask.sa_data[2]=mask[0];
		ifr.ifr_ifru.ifru_netmask.sa_data[3]=mask[1];
		ifr.ifr_ifru.ifru_netmask.sa_data[4]=mask[2];
		ifr.ifr_ifru.ifru_netmask.sa_data[5]=mask[3];

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
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		close(mSocket);
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
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		close(mSocket);
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
	mDHCPSniffer->Cancel();
	delete mDHCPSniffer;
	delete mDHCPsrv;
	try
	{
		if (mPacketCollector != NULL)
		{
			delete mPacketCollector;
			mPacketCollector = NULL;
		}

		close(mSocket);
		free(mInterfaceName);
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		close(mSocket);
	}
}

bool CPhysicalConnection::IsPacketEmpty(char* buffer)
{
	try
	{
		int sum = 0;
		for (int i = 0; i < ETH_FRAME_LEN; ++i)
		{
			sum += buffer[i];
			if (sum > 0)
			{
				return false;
			}
		}
		return true;
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		close(mSocket);
	}
	return false;
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

