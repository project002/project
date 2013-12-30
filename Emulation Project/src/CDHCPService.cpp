/*
 * CDHCPService.cpp
 *
 *  Created on: Dec 26, 2013
 *      Author: root
 */

#include "CDHCPService.h"

string CDHCPService::DHCP_FILTER = "udp and src port 68 and dst port 67";
string CDHCPService::DHCP_FILTER_RCV = "udp and src port 68 and dst port 67";
vector< pair< string,bool > > CDHCPService::IP_TABLE = vector< pair< string,bool > >();

CDHCPService::CDHCPService(char* iFaceName,const uint8_t* subNetName):
		miFaceName(iFaceName),mSubnatName(subNetName)
{

	//init ip table
	int table_sz = getIPTableSizeFromSubnet(mSubnatName);
	fillupIPTable(table_sz);
	cout << "ip table filled" << endl;
	mHandshakeIP = getIPAddr(); //allocate a IP address for the handshake
}



const char* CDHCPService::getIPAddr()
{
	vector< pair< string,bool > >::iterator it = CDHCPService::IP_TABLE.begin();
	for (;it!=CDHCPService::IP_TABLE.end();++it)
	{
		if (!it->second) //not taken
		{
			it->second = true; //mark as taken
			return it->first.c_str();
		}
	}
	return "";
}

void CDHCPService::fillupIPTable(int tableSize)
{
	stringstream ss;
	for (int i=0;i<tableSize;++i)
	{
		ss << "10.0.0." <<  (i+1);
		CDHCPService::IP_TABLE.push_back(pair<string,bool>(ss.str(),false));
		ss.str("");
	}
}


int CDHCPService::getIPTableSizeFromSubnet(const uint8_t* subnet)
{
	int sz = 0; //number of ip address
	int shifts = 0; //counter for number of shifts for each octet
	int max_shifts = 8; //maximum number of shifts for octet (until zero)
	uint8_t octet;
	for (int i=0;i<IPV4_ADDR_SZ;++i)
	{
		octet = subnet[i];
		shifts = max_shifts;
		while (octet>0)
		{
			--shifts;
			octet = (octet << 1);
		}
		sz += shifts;
	}
	//2^sz-2 will give all the available IP addresses
	//without broadcast and all zeros address
	return (int)(pow(2,(double)sz)-2);
}


void CDHCPService::start(Packet* packet)
{
	startDHCPhandshake(packet);
}

void CDHCPService::startDHCPhandshake(Packet* sniff_packet)
{
	cout << "Started Handshake Offering " << mHandshakeIP << endl;
	word XID = 0;
	string clientMAC("");
	try
	{
		DHCP* dhcp_packet = new DHCP();
		getDHCP(sniff_packet,OPCODE_REQ,MT_DISCOVER,dhcp_packet);
		XID = dhcp_packet->GetTransactionID();
		clientMAC = dhcp_packet->GetClientMAC();

		delete dhcp_packet;
	}
	catch (CException & error)
	{
		cerr << "When Parsing DHCP Packet: " << error.what() << endl;
		return;
	}

	string iface(miFaceName);

	Packet DHCP_OFFER;

	buildDHCP_OFFER(XID,clientMAC,&DHCP_OFFER);

	/* Send the packet */
	Packet* rcv = DHCP_OFFER.SendRecv(iface,0.2, 2,DHCP_FILTER_RCV);

	if (rcv)
	{
		try
		{
			/* Received DHCP layer */
			DHCP* dhcp_rcv = new DHCP();

			getDHCP(rcv,OPCODE_REQ,MT_REQUEST,dhcp_rcv);

			Packet DHCP_ACK;

			buildDHCP_ACK(XID,clientMAC,&DHCP_ACK);

			DHCP_ACK.Send(iface);

			delete dhcp_rcv;

			cout << "Handshake Finished With Setting " << mHandshakeIP << endl;

		}
		catch (CException & error)
		{
			cerr << "When Parsing DHCP Packet That Was Received After OFFER: " << error.what() << endl;
			return;
		}
	}
	else
	{
		cout << "[@] No response from any DHCP server" << endl;
		CleanCrafter();
	}

}

void CDHCPService::getDHCP(Packet* sniff_packet,byte OpCode,byte MsgType,DHCP* dhcp_rcv)
{
	/* Received DHCP layer */
	dhcp_rcv->FromRaw(*GetRawLayer(*sniff_packet));

	byte _OpCode = dhcp_rcv->GetOperationCode();
	if (_OpCode != OpCode)
	{
		throw CException("Wrong Opcode Found in Packet");
	}

	//find the DHCP message type option
	vector<DHCPOptions*>::const_iterator it_opt = dhcp_rcv->Options.begin();
	for(; it_opt != dhcp_rcv->Options.end() ; it_opt++)
	{
		if((*it_opt)->GetCode() == DHCPOptions::DHCPMsgType)
		{
			if ((*it_opt)->GetNumber() !=  MsgType)
			{
				stringstream ss;
				ss << "Wrong Message Type Found in Packet. Expected : " << (int)MsgType << " Got:" << (*it_opt)->GetNumber();
				throw CException(ss.str());
			}
		}
	}
}


void CDHCPService::buildDHCP_OFFER(word XID,string &clientMAC,Packet* DHCP_OFFER)
{
	/* Get interface properties - IP & MAC */
	string iface(miFaceName);
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
	dhcp_header.SetTransactionID(XID);
	dhcp_header.SetClientMAC(clientMAC);
	dhcp_header.Options.push_back(
			CreateDHCPOption(DHCPOptions::DHCPMsgType,
					DHCPOptions::DHCPOFFER, DHCPOptions::BYTE));
	dhcp_header.Options.push_back(
			CreateDHCPOption(DHCPOptions::SubnetMask, (byte*) mSubnatName,sizeof(uint8_t)*IPv4_ALEN));


	IPsInsert.push_back(MyIP);
	dhcp_header.Options.push_back(
			CreateDHCPOption(DHCPOptions::Router, IPsInsert));

	dhcp_header.Options.push_back(
			CreateDHCPOption(DHCPOptions::DHCPServerId, IPsInsert));

	dhcp_header.Options.push_back(
			CreateDHCPOption(DHCPOptions::DomainServer, IPsInsert));

	IPsInsert.clear();
	IPsInsert.push_back("7fffffff"); //TODO: what is this?
	dhcp_header.Options.push_back(
			CreateDHCPOption(DHCPOptions::AddressTime, IPsInsert));

	dhcp_header.SetClientIP("0.0.0.0");
	dhcp_header.SetYourIP(mHandshakeIP);
	dhcp_header.SetServerIP("0.0.0.0");
	dhcp_header.SetGatewayIP("0.0.0.0");

	(*DHCP_OFFER) = ether_header / ip_header / udp_header / dhcp_header;
}


void CDHCPService::buildDHCP_ACK(word XID,string &clientMAC,Packet* DHCP_ACK)
{
	/* Get interface properties - IP & MAC */
	string iface(miFaceName);
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
	dhcp_header.SetTransactionID(XID);
	dhcp_header.SetClientMAC(clientMAC);
	dhcp_header.Options.push_back(
			CreateDHCPOption(DHCPOptions::DHCPMsgType,
					DHCPOptions::DHCPACK, DHCPOptions::BYTE));
	dhcp_header.Options.push_back(
			CreateDHCPOption(DHCPOptions::SubnetMask, (byte*) mSubnatName,sizeof(uint8_t)*IPv4_ALEN));


	IPsInsert.push_back(MyIP);
	dhcp_header.Options.push_back(
			CreateDHCPOption(DHCPOptions::Router, IPsInsert));

	dhcp_header.Options.push_back(
			CreateDHCPOption(DHCPOptions::DHCPServerId, IPsInsert));

	dhcp_header.Options.push_back(
			CreateDHCPOption(DHCPOptions::DomainServer, IPsInsert));

	IPsInsert.clear();
	IPsInsert.push_back("7fffffff"); //TODO: what is this?
	dhcp_header.Options.push_back(
			CreateDHCPOption(DHCPOptions::AddressTime, IPsInsert));

	dhcp_header.SetClientIP("0.0.0.0");
	dhcp_header.SetYourIP(mHandshakeIP);
	dhcp_header.SetServerIP("0.0.0.0");
	dhcp_header.SetGatewayIP("0.0.0.0");

	(*DHCP_ACK) = ether_header / ip_header / udp_header / dhcp_header;
}


CDHCPService::~CDHCPService()
{
	// TODO Auto-generated destructor stub
}


