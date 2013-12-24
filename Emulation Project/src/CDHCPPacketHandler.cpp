/*
 * CDHCPPacketHandler.cpp
 *
 *  Created on: Dec 24, 2013
 *      Author: root
 */

#include "CDHCPPacketHandler.h"

string CDHCPPacketHandler::mIPMaskAddress = string("");
char* CDHCPPacketHandler::mInterfaceName = 0;

void CDHCPPacketHandler::startDHCPhandshake(Packet* sniff_packet, void* user)
{
	word XID = 0;
	try
	{
		XID = getXID(sniff_packet);
	}
	catch (CException & error)
	{
		cout << "When getting the transaction ID : " << error.what() << endl;
		return;
	}

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
	dhcp_header.SetTransactionID(XID);
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

word CDHCPPacketHandler::getXID(Packet* sniff_packet)
{
	/* Received DHCP layer */
	DHCP* dhcp_rcv = new DHCP();
	dhcp_rcv->FromRaw(*GetRawLayer(*sniff_packet));

	byte opCode = dhcp_rcv->GetOperationCode();
	if (opCode != 0x01)
	{throw new CException("Wrong Opcode in given packet");}

	//find the DHCP message type option
	vector<DHCPOptions*>::const_iterator it_opt = dhcp_rcv->Options.begin();
	for(; it_opt != dhcp_rcv->Options.end() ; it_opt++)
	{
		if((*it_opt)->GetCode() == DHCPOptions::DHCPMsgType)
		{
			if ((*it_opt)->GetNumber() !=  0x01)
			{throw new CException("Wrong Message Type in Packet");}
		}
	}
	return dhcp_rcv->GetTransactionID();
}
