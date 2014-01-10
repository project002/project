/*
 * CDHCPService.cpp
 *
 *  Created on: Dec 26, 2013
 *      Author: root
 */

#include "CDHCPService.h"

#define BROADCAST_MAC_ADDRESS "ff:ff:ff:ff:ff:ff"
#define BROADCAST_IP_ADDRESS "255.255.255.255"
#define EMPTY_IP_ADDRESS "0.0.0.0"
string CDHCPService::DHCP_FILTER = "udp and src port 68 and dst port 67";
string CDHCPService::DHCP_FILTER_RCV = "udp and src port 68 and dst port 67";
vector< string > CDHCPService::IP_TABLE = vector<string>();
bool CDHCPService::INIT_TABLE = true; //mark the first time to init the IP Table

CDHCPService::CDHCPService(char* iFaceName,const uint8_t* subNetName):
		mLocal_Table(vector< pair<string,string> >()),miFaceName(iFaceName),mSubnetName(subNetName),DEF_IPv4("0.0.0.0")
{

	int table_sz = 0;
	if (CDHCPService::INIT_TABLE) //if the table need to be initialized
	{
		//init ip table
		table_sz = getIPTableSizeFromSubnet(mSubnetName);
		fillupIPTable(table_sz);
		CDHCPService::INIT_TABLE = false; //don't init in next service
		cout << "ip table init complete" << endl;
	}
	mHandshakeIP = DEF_IPv4;
}



const char* CDHCPService::getIPAddr(string MAC)
{
	if (CDHCPService::IP_TABLE.empty())
	{
		throw new CException("DHCP table is empty, no More ip addresses available");
	}
	string addr = CDHCPService::IP_TABLE.back();
	CDHCPService::IP_TABLE.pop_back();
	mLocal_Table.push_back(pair<string,string> (addr,MAC) );
	cout << "last added ip address: " << mLocal_Table.back().first << " for MAC: " << mLocal_Table.back().second << endl;
	return mLocal_Table.back().first.c_str();
}

void CDHCPService::releaseIPAddr(string IPv4)
{
	//if default ip address do nothing
	if (IPv4.compare(DEF_IPv4)==0) {return;}
	//check duplicates
	vector<string>::iterator ip_it = find(CDHCPService::IP_TABLE.begin(),CDHCPService::IP_TABLE.end(),IPv4);
	//vector<pair< string,string> >::iterator cur_it = find(mLocal_Table.begin(),mLocal_Table.end(),IPv4);
	//find with pair:
	vector<pair< string,string> >::iterator cur_it = mLocal_Table.begin();
	for (;cur_it!=mLocal_Table.end();++cur_it)
	{if (cur_it->first.compare(IPv4)==0) {break;}}

	//add the IPv4 back to the IP_TABLE
	if (ip_it==CDHCPService::IP_TABLE.end()) {CDHCPService::IP_TABLE.push_back(IPv4);}

	//remove the IPv4 from the local ip table
	if (cur_it!=mLocal_Table.end()) {mLocal_Table.erase(cur_it);}
}

vector< pair<string,string> >& CDHCPService::getAllocatedIPs()
{
	return CDHCPService::mLocal_Table;
}

void CDHCPService::fillupIPTable(int tableSize)
{
	stringstream ss;
	for (int i=0;i<tableSize;++i)
	{
		ss << "10.0.0." <<  (i+1);
		CDHCPService::IP_TABLE.push_back(ss.str());
		ss.str("");
	}
	//reverse the vector so the logically first address will be in the end
	//so when used with pop_back we'll get the logically initial ip addresses
	reverse(CDHCPService::IP_TABLE.begin(),CDHCPService::IP_TABLE.end());
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
	word XID = 0;
	string clientMAC("");
	try
	{
		DHCP* dhcp_packet = new DHCP();
		getDHCP(sniff_packet,OPCODE_REQ,MT_DISCOVER,dhcp_packet);
		XID = dhcp_packet->GetTransactionID();
		clientMAC = dhcp_packet->GetClientMAC();
		mHandshakeIP = getIPAddr(clientMAC); //allocate a IP address for the handshake
		cout << "Started Handshake Offering " << mHandshakeIP << endl;
		delete dhcp_packet;
	}
	catch (CException & error)
	{
		cerr << "When Parsing DHCP Packet: " << error.what() << endl;
		releaseIPAddr(mHandshakeIP);
		return;
	}

	string iface(miFaceName);

	Packet DHCP_OFFER;

	buildDHCP_OFFER(XID,clientMAC,&DHCP_OFFER);

	/* Send the packet */
	Packet* rcv = DHCP_OFFER.SendRecv(iface,0.3, 2,DHCP_FILTER_RCV);

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

			mHandshakeIP = DEF_IPv4; //re-init the handshake ip

		}
		catch (CException & error)
		{
			cerr << "When Parsing DHCP Packet That Was Received After OFFER: " << error.what() << endl;
			releaseIPAddr(mHandshakeIP);
			return;
		}
	}
	else
	{
		cout << "[@] No response to DHCP offer with ip " << mHandshakeIP << endl;
		releaseIPAddr(mHandshakeIP);
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
	BuildDHCPPacket(XID,clientMAC,DHCP_OFFER,DHCPOptions::DHCPOFFER);
}

void CDHCPService::buildDHCP_ACK(word XID,string &clientMAC,Packet* DHCP_ACK)
{
	BuildDHCPPacket(XID,clientMAC,DHCP_ACK,DHCPOptions::DHCPACK);
}

void CDHCPService::BuildDHCPPacket(word XID,string &clientMAC,Packet* DHCP_PACKET,const word MsgType)
{
	try
	{
		/* Get interface properties - IP & MAC */
			string iface(miFaceName);
			string MyIP = GetMyIP(iface);
			string MyMAC = GetMyMAC(iface);

			Ethernet ether_header;
			ether_header.SetSourceMAC(MyMAC);         // <-- Set our MAC as a source
			ether_header.SetDestinationMAC(BROADCAST_MAC_ADDRESS); // <-- Set broadcast address
			IP ip_header;
			ip_header.SetSourceIP(MyIP);
			ip_header.SetDestinationIP(BROADCAST_IP_ADDRESS);

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
					CreateDHCPOption(DHCPOptions::SubnetMask, (byte*) mSubnetName,sizeof(uint8_t)*IPv4_ALEN));


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

			dhcp_header.SetClientIP(EMPTY_IP_ADDRESS);
			dhcp_header.SetYourIP(mHandshakeIP);
			dhcp_header.SetServerIP(EMPTY_IP_ADDRESS);
			dhcp_header.SetGatewayIP(EMPTY_IP_ADDRESS);

			(*DHCP_PACKET) = ether_header / ip_header / udp_header / dhcp_header;
	}

	catch (CException & error)
	{
		cerr << error.what() << endl;
		releaseIPAddr(mHandshakeIP);
		return;
	}
}
CDHCPService::~CDHCPService()
{
	//clear all the allocated ip addresses by this service
	vector<pair <string,string> >::iterator it = mLocal_Table.begin();
	for(;it!=mLocal_Table.end();++it)
	{
		CDHCPService::IP_TABLE.push_back(it->first);
	}
	mLocal_Table.clear();
}


