/*
 * CPhysicalConnection.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#include "CPhysicalConnection.h"
#include "CDHCPPacketHandler.h"

CPhysicalConnection::CPhysicalConnection(struct ifaddrs* device):mSocket(-1),mPacketCollector(NULL),mInterfaceName(NULL)
{
	try
	{
		// Init the structs with 0's
		InitStructs(device);

		// Configuring the socket
		ConfigureSocket(device);

		// Get the interface information
		GetInterfaceInformation();

		SetNetmask();
		//start snifing for DHCP Discover packets
		SniffDHCPPackets();
//		SetConnectedDevicesIPAddresses(NULL,NULL);
		//GetConnectedDevicesMACAddresses();
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		close(mSocket);
	}

}

void CPhysicalConnection::SniffDHCPPackets()
{
	string iFace(mInterfaceName);
	CDHCPPacketHandler::mInterfaceName = mInterfaceName;
	CDHCPPacketHandler::mIPMaskAddress = mIPMaskAddress;
	CDHCPPacketHandler::mIPAddressSet = "10.0.0.2";
	Sniffer sniff("udp and src port 68 and dst port 67",iFace,CDHCPPacketHandler::startDHCPhandshake);

	sniff.Capture();
}

/**
 * function uses DHCP packets to set clients IP addresses per request
 */
void CPhysicalConnection::SetConnectedDevicesIPAddresses(Packet* sniff_packet, void* user)
{
	try
	{
		/* Get interface properties - IP & MAC */
		string iface = mInterfaceName;
		string MyIP = GetMyIP(iface);
		string MyMAC = GetMyMAC(iface);

		Ethernet ether_header;
		ether_header.SetSourceMAC(MyMAC);         // <-- Set our MAC as a source
		ether_header.SetDestinationMAC("ff:ff:ff:ff:ff:ff"); // <-- Set broadcast address
		IP ip_header;
		ip_header.SetSourceIP(MyIP);
		ip_header.SetDestinationIP("255.255.255.255");
		UDP udp_header;
		udp_header.SetSrcPort(67);
		udp_header.SetDstPort(68);
		DHCP dhcp_header;
		vector<std::string> IPsInsert;
		dhcp_header.SetOperationCode(DHCP::Reply);
		dhcp_header.SetFlags(0x0);
//		dhcp_header.SetTransactionID(RNG32());
		dhcp_header.SetTransactionID(0xe0cb1350);
		dhcp_header.SetClientMAC("00:24:81:22:A0:B5");
		dhcp_header.Options.push_back(
				CreateDHCPOption(DHCPOptions::DHCPMsgType,
						DHCPOptions::DHCPOFFER, DHCPOptions::BYTE));
		dhcp_header.Options.push_back(
				CreateDHCPOption(DHCPOptions::SubnetMask, mIPMaskAddress));


		IPsInsert.push_back(MyIP);
		dhcp_header.Options.push_back(
				CreateDHCPOption(DHCPOptions::Router, IPsInsert));

		dhcp_header.Options.push_back(
				CreateDHCPOption(DHCPOptions::DHCPServerId, IPsInsert));
		dhcp_header.Options.push_back(
				CreateDHCPOption(DHCPOptions::DomainServer, IPsInsert));

		IPsInsert.clear();
		IPsInsert.push_back("7fffffff");
		dhcp_header.Options.push_back(
				CreateDHCPOption(DHCPOptions::AddressTime, IPsInsert));

		dhcp_header.SetClientIP("0.0.0.0");
		dhcp_header.SetYourIP("10.0.0.4");
		dhcp_header.SetServerIP("0.0.0.0");
		dhcp_header.SetGatewayIP("0.0.0.0");
		Packet packet = ether_header / ip_header / udp_header / dhcp_header;
		/* Send the packet */
		Packet* rcv = packet.SendRecv(iface, 0.01, 3000,
				"udp and src port 68 and dst port 67");

		/* Received DHCP layer */
		DHCP* dhcp_rcv = new DHCP();

		if (rcv)
		{
			/*
			 * An application protocol is always get from the network as a raw layer.
			 */
			dhcp_rcv->FromRaw(*GetRawLayer(*rcv));
			/* Print the response to STDOUT to see how it looks like */
			cout
					<< "[@] ---------- DHCP response from server after a DISCOVERY message : "
					<< endl;
			dhcp_rcv->Print();
			cout << "[@] ---------- " << endl;
		}
		else
		{
			cout << "[@] No response from any DHCP server" << endl;
			CleanCrafter();
		}

	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		close(mSocket);
	}
}
/**
 * Initialization level function
 * The function sends an reversed ARP packet in order to retrieve all connected devices IP addresses
 */
void CPhysicalConnection::GetConnectedDevicesMACAddresses()
{
	try
	{
		/* Get interface properties - IP & MAC */
		string iface = mInterfaceName;
		string MyIP = GetMyIP(iface);
		string MyMAC = GetMyMAC(iface);

		/* Set Packets Headers*/
		Ethernet ether_header;
		ether_header.SetSourceMAC(MyMAC);         // <-- Set our MAC as a source
		ether_header.SetDestinationMAC("ff:ff:ff:ff:ff:ff"); // <-- Set broadcast address
		ARP arp_header;
		arp_header.SetOperation(ARP::Request); // <-- Set Operation (ARP Request)
		arp_header.SetSenderIP(MyIP);                // <-- Set our network data
		arp_header.SetSenderMAC(MyMAC);           // <-- Set our MAC as a sender

		/* ---------------------------------------------- */
		/* Define the network to scan */
		vector<string> net = GetIPs("10.0.0.*"); // <-- Create a container of IP addresses from a "wildcard"
		vector<string>::iterator ip_addr;                     // <-- Iterator
		/* Create a container of packet pointers to hold all the ARP requests */
		vector<Packet*> request_packets;
		/* Iterate to access each string that defines an IP address */
		for (ip_addr = net.begin(); ip_addr != net.end(); ip_addr++)
		{
			arp_header.SetTargetIP(*ip_addr); // <-- Set a destination IP address on ARP header
			Packet* packet = new Packet;
			packet->PushLayer(ether_header);
			packet->PushLayer(arp_header);
			request_packets.push_back(packet);
		}
		cout << "[@] Sending the ARP Requests. Wait..." << endl;
		/* Create a container of packet with the same size of the request container to hold the responses */
		vector<Packet*> replies_packets(request_packets.size());
		SendRecv(request_packets.begin(), request_packets.end(),
				replies_packets.begin(), iface, 0.1, 2, 48);

		cout << "[@] SendRecv function returns :-) " << endl;

		vector<Packet*>::iterator it_pck;
		for (it_pck = replies_packets.begin(); it_pck < replies_packets.end();
				it_pck++)
		{
			Packet* reply_packet = (*it_pck);
			if (reply_packet!=NULL)
			{
				ARP* arp_layer = reply_packet->GetLayer<ARP>();
				mConnectedDevicesIPv4Addresses.insert(TStrStrPair(arp_layer->GetSenderIP(),arp_layer->GetSenderMAC()));
				cout <<mConnectedDevicesIPv4Addresses[arp_layer->GetSenderIP()].c_str();
			}
		}
		for (it_pck = request_packets.begin(); it_pck < request_packets.end();
				it_pck++)
			delete (*it_pck);

		for (it_pck = replies_packets.begin(); it_pck < replies_packets.end();
				it_pck++)
			delete (*it_pck);


	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
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


	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
		close(mSocket);
	}
}

void CPhysicalConnection::SetNetmask()
{
	try
	{
		stringstream ss;
		struct ifreq ifr;
		memset(&ifr,0,sizeof(struct ifreq));
		strncpy(ifr.ifr_ifrn.ifrn_name,mInterfaceName,IFNAMSIZ);
		ifr.ifr_ifru.ifru_netmask.sa_family = AF_INET;
		ifr.ifr_ifru.ifru_addr.sa_family = AF_INET;

		// Setting random IP address for the connection
		ifr.ifr_ifru.ifru_addr.sa_data[2]=/*rand()%255*/10;
		ifr.ifr_ifru.ifru_addr.sa_data[3]=/*rand()%255*/0;
		ifr.ifr_ifru.ifru_addr.sa_data[4]=/*rand()%255*/0;
		ifr.ifr_ifru.ifru_addr.sa_data[5]=/*rand()%255*/5;

		if (ioctl(mSocket, SIOCSIFADDR, &ifr) < 0)
		{
			perror("SIOCSIFNETMASK");
			throw(CException("Can't set a new IP"));
		}
		ss.clear();

		ifr.ifr_ifru.ifru_netmask.sa_data[2]=0xff;
		ifr.ifr_ifru.ifru_netmask.sa_data[3]=0xff;
		ifr.ifr_ifru.ifru_netmask.sa_data[4]=0xff;
		ifr.ifr_ifru.ifru_netmask.sa_data[5]=0;

		ss<<ifr.ifr_ifru.ifru_netmask.sa_data[2]<<ifr.ifr_ifru.ifru_netmask.sa_data[3] << ifr.ifr_ifru.ifru_netmask.sa_data[4]<< ifr.ifr_ifru.ifru_netmask.sa_data[5];
		mIPMaskAddress= ss.str();
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

