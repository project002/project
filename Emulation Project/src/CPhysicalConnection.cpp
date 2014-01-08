/*
 * CPhysicalConnection.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#include "CPhysicalConnection.h"
#define ERROR_MSG_SETTING_IP "Can't set a new IP"
#define ERROR_MSG_SETTING_NET_MASK "Can't set a new net mask - Check if cable is connected"
#define ERROR_MSG_SETTING_FLAGS "Can't get network flags"
#define ERROR_MSG_GET_INTERFACE_INDEX "Failed to retrieve interface index"
#define IFREQ_STRUCT_IP_OFFSET 2

/**
 * Class C-tor, calls all sub functions to create the physical link properly.
 *
 * @param device ifaddrs struct to open the socket according to.
 */
CPhysicalConnection::CPhysicalConnection(struct ifaddrs* device) :
		CConnection(), mInterfaceName(NULL)
{

	try
	{
		// Init the structs with 0's and get the interface name
		InitStructs(device);

		// Configuring the socket
		ConfigureSocket(device);

		// Get the interface information
		GetInterfaceInformation();

		//Set the Subnet Mask
		setMaskAddress();

		//Initiating the DHCP service in order to get the IP's list
		mDHCPsrv = new CDHCPService(mInterfaceName,mIPMaskAddress->getIpArr());
		string interface_mac = GetMyMAC(mInterfaceName);
		const char* getway_addr = mDHCPsrv->getIPAddr(interface_mac);
		mGetwayAddress = new CUIPV4(string(getway_addr));

		//Setting subnet masking
		SetNetmask(mGetwayAddress,mIPMaskAddress);

		//Starting the DHCP service
		startDHCPService();
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		close(mSocket);
	}

}

/**
 * Starting the DHCP service
 */
void CPhysicalConnection::startDHCPService()
{

	cout << "DHCP service started..." << endl;
	mDHCPSniffer = new Sniffer(CDHCPService::DHCP_FILTER,string(mInterfaceName),runDHCPService);

	mDHCPSniffer->Spawn(-1,static_cast<void*> (mDHCPsrv));
	//	mDHCPSniffer->Capture(-1,static_cast<void*> (mDHCPsrv));
}

bool CPhysicalConnection::SendPacket(Packet* packet) const
{
	try
	{
		cout << "sending packet" << endl;
		packet->Print();
		Ethernet* eth_layer  = packet->GetLayer<Ethernet>();
		IP* ip_layer  = packet->GetLayer<IP>();
		ip_layer->SetTTL(ip_layer->GetTTL()-1);
		eth_layer->SetSourceMAC(GetMyMAC(mInterfaceName));
		//TODO: get destination MAC address - make DHCP service save the MAC addresses
		if (ip_layer->GetTTL() >= 1)
			return (bool) packet->Send(mInterfaceName);
		return false;
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		close(mSocket);
	}
	return false;
}

/**
 * Setting subnet masking as 255.255.255.0
 */
void CPhysicalConnection::setMaskAddress()
{
	uint8_t subnet[] = {0xff,0xff,0xff,0x00};
	mIPMaskAddress = new CUIPV4(subnet);
}

/**
 * Getting interface index number for future use of getting
 * and setting socket flags and more.
 */
void CPhysicalConnection::GetInterfaceInformation()
{
	try
	{
		// Getting the interface index
		if (ioctl(mSocket, SIOGIFINDEX, &mIfreq) < 0)
		{
			throw CException(ERROR_MSG_GET_INTERFACE_INDEX);
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

/**
 * Setting subnet masking using OS commands
 *
 * @param getway_addr the gateway to be set
 * @param mIPMaskAddress the ip masking to be set
 */
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

		ifr.ifr_ifru.ifru_addr.sa_data[0+IFREQ_STRUCT_IP_OFFSET]=addr[0];
		ifr.ifr_ifru.ifru_addr.sa_data[1+IFREQ_STRUCT_IP_OFFSET]=addr[1];
		ifr.ifr_ifru.ifru_addr.sa_data[2+IFREQ_STRUCT_IP_OFFSET]=addr[2];
		ifr.ifr_ifru.ifru_addr.sa_data[3+IFREQ_STRUCT_IP_OFFSET]=addr[3];

		if (ioctl(mSocket, SIOCSIFADDR, &ifr) < 0)
		{
			perror("SIOCSIFNETMASK");
			throw(CException(ERROR_MSG_SETTING_IP));
		}
		ss.clear();

		const uint8_t* mask = mIPMaskAddress->getIpArr();

		ifr.ifr_ifru.ifru_netmask.sa_data[0+IFREQ_STRUCT_IP_OFFSET]=mask[0];
		ifr.ifr_ifru.ifru_netmask.sa_data[1+IFREQ_STRUCT_IP_OFFSET]=mask[1];
		ifr.ifr_ifru.ifru_netmask.sa_data[2+IFREQ_STRUCT_IP_OFFSET]=mask[2];
		ifr.ifr_ifru.ifru_netmask.sa_data[3+IFREQ_STRUCT_IP_OFFSET]=mask[3];

		if (ioctl(mSocket, SIOCSIFNETMASK, &ifr) < 0)
		{
			perror("SIOCSIFNETMASK");
			throw(CException(ERROR_MSG_SETTING_NET_MASK));
		}

		if(ioctl(mSocket,SIOCGIFFLAGS,&ifr)<0)
		{
			perror("SIOCGIFFLAGS");
			throw(CException(ERROR_MSG_SETTING_FLAGS));
		}
		ifr.ifr_ifru.ifru_flags |= (IFF_UP | IFF_RUNNING);
		if(ioctl(mSocket,SIOCSIFFLAGS,&ifr)<0)
		{
			perror("SIOCSIFFLAGS");
			throw(CException(ERROR_MSG_SETTING_FLAGS));
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

vector< pair<string,string> >& CPhysicalConnection::GetTable()const
{
	return mDHCPsrv->getAllocatedIPs();
}

Crafter::Packet* CPhysicalConnection::GetPacket()
{
	bool fail_cond_err = false;
	bool fail_cond_empty = false;
	memset(buffer,0,ETH_FRAME_LEN);
	ethPacket = new Packet();
	try
	{
		ssize_t recvSize=recvfrom(mSocket,buffer,ETH_FRAME_LEN,0,NULL,NULL);
		fail_cond_err = (recvSize == -1 && errno != EAGAIN && errno != EWOULDBLOCK);
		fail_cond_empty = (recvSize < 1);

		if (fail_cond_err)
		{
			delete ethPacket;
			throw CException("fatal error on receive from socket");
		}

		if (fail_cond_empty)
		{
			delete ethPacket;
			return NULL;
		}

		ethPacket->PacketFromEthernet(buffer,recvSize);
		return ethPacket;
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		close(mSocket);
	}
	return NULL;
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

