/*
 * CDHCPPacketHandler.h
 *
 *  Created on: Dec 24, 2013
 *      Author: root
 */

#ifndef CDHCPPACKETHANDLER_H_
#define CDHCPPACKETHANDLER_H_

#include "BasicIncludes.h"

class CDHCPPacketHandler
{
public:
	//TODO: find these consts in the crafeter lib
	static const byte OPCODE_REQ  = 0x01; //Opcode for Request
	static const byte OPCODE_REP  = 0x01; //Opcode for Replay
	static const byte MT_DISCOVER = 0x01; //Message Type Discover
	static const byte MT_REQUEST  = 0x03; //Message Type REQUEST
	static const byte MT_ACK      = 0x05; //Message Type ACK on DHCP Request

	static char* mInterfaceName;
	static string mIPMaskAddress;
	static string mIPAddressSet; //the IP address to be offered to the client

	static void startDHCPhandshake(Packet* sniff_packet, void* user);

	/**
	 * returns a DHCP Packet object in dhcp_rcv argument
	 * if the given Packet is not a valid DHCP Packet
	 * according to the given parameters an appropriate
	 *  CExeption is thrown
	 * @param sniff_packet some packet
	 * @param OpCode the OpCode to check againts the given packet
	 * @param MsgType the DHCP message type to check against the given packet
	 * @param dhcp_rcv where the valid DHCP packet will be stored
	 */
	static void getDHCP(Packet* sniff_packet,byte OpCode,byte MsgType,DHCP* dhcp_rcv);

	/**
	 * builds an offer DHCP replay
	 * @param XID the transaction ID of the previous DISCOVERY
	 * @param clientMAC the MAC address of the machine that sent DISCOVERY
	 * @param DHCP_OFFER where the built packet will be stored
	 */
	static void buildDHCP_OFFER(word XID,string &clientMAC,Packet* DHCP_OFFER);

	/**
	 * builds an ack DHCP replay
	 * @param XID the transaction ID of the previous DISCOVERY
	 * @param clientMAC the MAC address of the machine that sent DISCOVERY
	 * @param DHCP_ACK where the built packet will be stored
	 */
	static void buildDHCP_ACK(word XID,string &clientMAC,Packet* DHCP_ACK);

};

#endif /* CDHCPPACKETHANDLER_H_ */
