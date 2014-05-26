/*
 * CPhysicalConnection.h
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#ifndef CPHYSICALCONNECTION_H_
#define CPHYSICALCONNECTION_H_
#include "BasicIncludes.h"

#include "CPacketCollector.h"
#include "CDHCPService.h"
#include "H_SNIFFER_FUNC.h"
#include "CConnection.h"
#include "CSocketNotReadyException.h"

#define MAX_NUMBER_OF_COMPUTERS_ON_NET 32

/**
 * A connection class of a physical connection
 */
class CPhysicalConnection:public CConnection
{
public:

	/**
	 * Constructor - Initialization of the physical connection, including sockets, addresses and more
	 * @param device the device must have a non-NULL ifa_addr
	 */
	CPhysicalConnection(struct ifaddrs* device);

	/**
	 * D-tor - deleting members and closing sockets
	 */
	virtual ~CPhysicalConnection();

	/**
	 * Retrieving the routing tables from each connection type
	 * @return pairs vector of IPv4 and MAC addresses achievable by the current connection
	 */
	virtual vector< pair<string,string> >& GetTable();

	/**
	 * Retrieving of packets from a physical connection
	 * @param routerNumber this value in a physical connection is ignored
	 * @return first packet in the packet collector's FIFO
	 */
	virtual Crafter::Packet* GetPacket( int fromPacketCollector=NO_CHOICE_OF_PACKET_COLLECTOR);

	/**
	 * Sending of packets from a virtual connection
	 * @param packet The packet to be sent
	 * @param this value in a physical connection is ignored
	 */
	virtual void SendPacket(Packet* packet,int routerNumber=NO_CHOICE_OF_PACKET_COLLECTOR);

	/**
	 * Retrieving the interface name i.e eth0 for socket handling
	 * @return
	 */
	const char* getInterfaceName() const{return mInterfaceName;}

	/**
	 * Getter of the connection's IPv4 address
	 * @return
	 */
	CUIPV4*& getGetwayAddress()  {return mGetwayAddress;}

	/**
	 * Setter of the connection's IPv4 address
	 */
	void setGetwayAddress( CUIPV4*& getwayAddress){mGetwayAddress = getwayAddress;}

	/**
	 * Getter of the connection's MAC address
	 * @return
	 */
	virtual string GetMAC()const;

	/**
	 * Checking if the current connection is physical or virtual, saving time on dynamic casting
	 * since it is needed in order to determine the report finishing of each packet
	 * @return true if the connection is physical
	 */
	bool isPhysical()const{return true;}

private:

	/**
	 * Starts the DHCP service on the current
	 */
	void startDHCPService();

	/**
	 * Setting NetMask so the socket would listen to the correct IP addresses
	 * @param getway_addr
	 * @param mIPMaskAddress
	 */
	void SetNetmask(CUIPV4* getway_addr, CUIPV4* mIPMaskAddress);

	/**
	 * Init the OS calls structs before initializing the device
	 * @param device
	 */
	void InitStructs(struct ifaddrs* device);

	/**
	 * Configuring the soscket
	 * @param device
	 */
	void ConfigureSocket(struct ifaddrs* device);

	/**
	 * Updating the interface ID using OS calls
	 */
	void GetInterfaceInformation();

	/**
	 * Generating the new Net Mask to be set
	 */
	void setMaskAddress();

	/**
	 * Starting the DHCP service
	 */
	void InitiateDHCPService();

	/**
	 * members for the OS calls
	 */
	int mInterfaceIndex;
	CUIPV4* mIPMaskAddress;
	CUIPV4* mGetwayAddress;
	struct ifreq mIfreq;

	char * mInterfaceName;

	//DHCPServices for Physical Connection
	CDHCPService* mDHCPsrv;
	Sniffer* mDHCPSniffer;
	Sniffer* mIPSniffer;

	//declerations for packet recieves
	byte buffer[ETH_FRAME_LEN];
	Packet* ethPacket;
};

#endif /* CPHYSICALCONNECTION_H_ */
