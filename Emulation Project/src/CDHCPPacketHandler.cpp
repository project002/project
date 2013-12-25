/*
 * CDHCPPacketHandler.cpp
 *
 *  Created on: Dec 24, 2013
 *      Author: root
 */

#include "CDHCPPacketHandler.h"

string CDHCPPacketHandler::mIPMaskAddress = string("");
string CDHCPPacketHandler::mIPAddressSet = string("");
char* CDHCPPacketHandler::mInterfaceName = 0;

void CDHCPPacketHandler::startDHCPhandshake(Packet* sniff_packet, void* user)
{
	cout << "processing packet" << endl;
	word XID = 0;
	string clientMAC("");
	try
	{
		DHCP* dhcp_packet = new DHCP();
		getDHCP(sniff_packet,OPCODE_REQ,MT_DISCOVER,dhcp_packet);
		XID = dhcp_packet->GetTransactionID();
		clientMAC = dhcp_packet->GetClientMAC();
//		cout << "got DIX:" << XID << " client MAC: " << clientMAC << endl;

		delete dhcp_packet;
	}
	catch (CException & error)
	{
		cerr << "When Parsing DHCP Packet: " << error.what() << endl;
		return;
	}

	string iface(mInterfaceName);

	Packet DHCP_OFFER;

	buildDHCP_OFFER(XID,clientMAC,&DHCP_OFFER);

	/* Send the packet */
	Packet* rcv = DHCP_OFFER.SendRecv(iface,0.1, 2,
			"udp and src port 68 and dst port 67");

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

		}
		catch (CException & error)
		{
			cerr << "When Parsing DHCP Packet That Was Recieved After OFFER: " << error.what() << endl;
			return;
		}
	}
	else
	{
		cout << "[@] No response from any DHCP server" << endl;
		CleanCrafter();
	}

}

void CDHCPPacketHandler::getDHCP(Packet* sniff_packet,byte OpCode,byte MsgType,DHCP* dhcp_rcv)
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
				throw CException("Wrong Message Type Found in Packet");
			}
		}
	}
}

void CDHCPPacketHandler::buildDHCP_OFFER(word XID,string &clientMAC,Packet* DHCP_OFFER)
{
	/* Get interface properties - IP & MAC */
	string iface(mInterfaceName);
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
			CreateDHCPOption(DHCPOptions::SubnetMask, mIPMaskAddress));


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
	dhcp_header.SetYourIP(mIPAddressSet);
	dhcp_header.SetServerIP("0.0.0.0");
	dhcp_header.SetGatewayIP("0.0.0.0");

	(*DHCP_OFFER) = ether_header / ip_header / udp_header / dhcp_header;
}


void CDHCPPacketHandler::buildDHCP_ACK(word XID,string &clientMAC,Packet* DHCP_ACK)
{
	/* Get interface properties - IP & MAC */
	string iface(mInterfaceName);
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
			CreateDHCPOption(DHCPOptions::SubnetMask, mIPMaskAddress));


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
	dhcp_header.SetYourIP(mIPAddressSet);
	dhcp_header.SetServerIP("0.0.0.0");
	dhcp_header.SetGatewayIP("0.0.0.0");

	(*DHCP_ACK) = ether_header / ip_header / udp_header / dhcp_header;
}


