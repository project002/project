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
#include <algorithm>
#include "CDHCPMsgTypeException.h"

#define IPV4_ADDR_SZ 4
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
	//holds if the IP_TABLE should be initiliazed
	//or left as is by the DHCP service
	static bool INIT_TABLE;
	/**
	 * holds all the free IPv4 left for
	 * any DHCP service
	 */
	static vector<string> IP_TABLE;
	/**
	 * holds all the IPv4 addresses
	 * this this specific instance of the DHCP
	 * service (with the appropriate interface) has allocated
	 * this also holds the MAC addresses that the ip is connected to
	 */
	vector<pair< string,string> > mLocal_Table;
	static const byte OPCODE_REQ  = 0x01; //Opcode for Request
	static const byte OPCODE_REP  = 0x02; //Opcode for Replay
	static const byte MT_DISCOVER = Crafter::DHCPOptions::DHCPDISCOVER; //Message Type Discover
	static const byte MT_INFORM   = Crafter::DHCPOptions::DHCPINFORM; //Message Type Information
	static const byte MT_RELEASE   = Crafter::DHCPOptions::DHCPRELEASE; //Message Type Release
	static const byte MT_REQUEST  = Crafter::DHCPOptions::DHCPREQUEST; //Message Type REQUEST
	static const byte MT_ACK      = Crafter::DHCPOptions::DHCPACK; //Message Type ACK on DHCP Request


	CDHCPService(char* iFaceName,const uint8_t* subNetName);

	void start(Packet* packet);

	/**
	 * returns a free IPv4 address in the subnet
	 * if the table is full an CEXception will be thrown
	 * @param MACaddr the MAC address to bind to this ip
	 * @return a free IPv4 address
	 */
	const char* getIPAddr(string MAC);

	/**
	 * release the given ip address back to the IP_TABLE
	 * implicitly also releases the MAC address binded to it
	 * @param IPv4 an ipv4 ip address
	 */
	void releaseIPAddr(string IPv4);


	/**
	 * returns a vector of all the allocated ip addresses
	 * in thos DHCP service (thus all the ips that are connected
	 * to the interface running this service)
	 * @return a vector of ip addresses in string form
	 */
	vector< pair<string,string> >& getAllocatedIPs();

	virtual ~CDHCPService();
private:
	char* miFaceName; //the interface name
	const uint8_t* mSubnetName; //the subnet ip mask
	const string DEF_IPv4;
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

	/**
	 * Handle the release IP address protocol
	 * @param release_packet a DHCP packet of release request
	 */
	void handleRelease(Packet* release_packet);

	/**
	 * Handle the request IP address protocol
	 * @param release_packet a DHCP packet of "request" request
	 */
	void handleRequest(Packet* request_packet);


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

	void BuildDHCPPacket(word XID,string &clientMAC,Packet* DHCP_PACKET,const word MsgType);
};

#endif /* CDHCPSERVICE_H_ */
