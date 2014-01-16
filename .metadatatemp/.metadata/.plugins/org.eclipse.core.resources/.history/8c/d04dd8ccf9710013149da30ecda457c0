/*
 * CDHCPService.h
 *
 *  Created on: Dec 26, 2013
 *      Author: root
 */

#ifndef CDHCPSERVICE_H_
#define CDHCPSERVICE_H_

#include "BasicIncludes.h"
#include <cmath>

/**
 * should be a function object that starts the DHCP
 * service in the emulation initialization
 */
class CDHCPService
{
public:
	//sniffer filter for DHCP packets
	static string DHCP_FILTER;
	//packet recieve filer
	static string DHCP_FILTER_RCV;
	/**
	 * ip table for available ip addresses
	 * each pair had the ip address (string)
	 * and if it's free or nor (bool)
	 */
	static vector< pair< string,bool > > IP_TABLE;
	//TODO: find these consts in the crafeter lib
	static const byte OPCODE_REQ  = 0x01; //Opcode for Request
	static const byte OPCODE_REP  = 0x02; //Opcode for Replay
	static const byte MT_DISCOVER = 0x01; //Message Type Discover
	static const byte MT_REQUEST  = 0x03; //Message Type REQUEST
	static const byte MT_ACK      = 0x05; //Message Type ACK on DHCP Request


	CDHCPService(char* iFaceName,const uint8_t* subNetName);

	void start(Packet* packet);

	/**
	 * returns a free IPv4 adrress in the subnet
	 * if the table is full an CEXception will be thrown
	 * @return a free IPv4 address
	 */
	const char* getIPAddr();

	virtual ~CDHCPService();
private:
	const int IPV4_ADDR_SZ = 4;
	char* miFaceName; //the interface name
	const uint8_t* mSubnatName; //the subnet ip mask
	string mHandshakeIP; //the offered ip adress for the current handshake

	/**
	 * @param subnet each char is the numeric (not ascii) valur of a octet
	 * @return the number of IP address that can be available with the subnet
	 */
	int getIPTableSizeFromSubnet(const uint8_t* subnet);

	/**
	 * this method will only make the ip address for subnet of type
	 * 255.255.255.0 (possible 254 valid ip addresses)
	 * TODO: make this method fillup the ip table with subnets other
	 * than 255.255.255.0
	 * @param tableSize size of the table
	 */
	void fillupIPTable(int tableSize);

	void startDHCPhandshake(Packet* sniff_packet);

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
	void getDHCP(Packet* sniff_packet,byte OpCode,byte MsgType,DHCP* dhcp_rcv);

	/**
	 * builds an offer DHCP replay
	 * @param XID the transaction ID of the previous DISCOVERY
	 * @param clientMAC the MAC address of the machine that sent DISCOVERY
	 * @param DHCP_OFFER where the built packet will be stored
	 */
	void buildDHCP_OFFER(word XID,string &clientMAC,Packet* DHCP_OFFER);

	/**
	 * builds an ack DHCP replay
	 * @param XID the transaction ID of the previous DISCOVERY
	 * @param clientMAC the MAC address of the machine that sent DISCOVERY
	 * @param DHCP_ACK where the built packet will be stored
	 */
	void buildDHCP_ACK(word XID,string &clientMAC,Packet* DHCP_ACK);

};

#endif /* CDHCPSERVICE_H_ */
