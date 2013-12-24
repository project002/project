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


#define MAX_NUMBER_OF_COMPUTERS_ON_NET 32
class CPhysicalConnection
{
public:
	/**
	 *
	 * @param device the device must have a non-NULL ifa_addr
	 */

	CPhysicalConnection(struct ifaddrs* device);
	virtual ~CPhysicalConnection();
	void SetNetmask();
	void Receive(){mPacketCollector->ReceivePackets();}
	void GetConnectedDevicesMACAddresses();
	/**
	 * used as handler for the sniffer method this is why he gets
	 * these arguments
	 * @param sniff_packet
	 * @param user
	 */
	void SetConnectedDevicesIPAddresses(Packet* sniff_packet, void* user);
	/**
	 * starts sniffing
	 */
	void SniffDHCPPackets();
private:
	void InitStructs(struct ifaddrs* device);
	void ConfigureSocket(struct ifaddrs* device);
	void GetInterfaceInformation();

	/**
	 * run a recv loop for a period of time specified
	 * @param period_len the duration of the rcv loop in seconds
	 */
	//void Receive(int unsigned period_len); //run a loop to catch packets
	bool IsPacketEmpty(char* packet);
	/**
	 * outputs the input of the packet content which is a ipv4 packet
	 * @param ipHead a pointer to ipv4 packet inside the ethernet packet
	 */
	//void read_ipv4(char* ipHead);
	int mSocket;
	int mInterfaceIndex;
	string mIPMaskAddress;
	struct ifreq mIfreq;
	CPacketCollector * mPacketCollector;
	map<string,string> mConnectedDevicesIPv4Addresses;
	char * mInterfaceName;
};

#endif /* CPHYSICALCONNECTION_H_ */
