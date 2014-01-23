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
		InitiateDHCPService();

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
void CPhysicalConnection::InitiateDHCPService()
{
	try
	{
		mDHCPsrv = new CDHCPService(mInterfaceName, mIPMaskAddress->getIpArr());
		string interface_mac = GetMyMAC(mInterfaceName);
		const char* getway_addr = mDHCPsrv->getIPAddr(interface_mac);
		mGetwayAddress = new CUIPV4(string(getway_addr));
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
		return (bool) packet->Send(mInterfaceName);
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		close(mSocket);
	}
	return false;
}

string CPhysicalConnection::GetMAC() const
{
	return GetMyMAC(mInterfaceName);
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
	if( mDHCPSniffer!=NULL)
	{
		delete mDHCPSniffer;
		mDHCPSniffer=NULL;
	}
	if (mDHCPsrv!=NULL)
	{
		delete mDHCPsrv;
		mDHCPsrv=NULL;
	}
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
			ethPacket=NULL;
			throw CException("fatal error on receive from socket");
		}

		if (fail_cond_empty)
		{
			delete ethPacket;
			ethPacket=NULL;
			return NULL;
		}
		ethPacket->PacketFromEthernet(buffer, recvSize);

		return ethPacket;
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		close(mSocket);
		throw CException("Emulation Terminated Socket Physical Socket Error");
	}
	return NULL;
}

