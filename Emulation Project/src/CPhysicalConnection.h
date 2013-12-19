/*
 * CPhysicalConnection.h
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#ifndef CPHYSICALCONNECTION_H_
#define CPHYSICALCONNECTION_H_
#include "BasicIncludes.h"
#include "CMacAddress.h"
#include "CPacketCollector.h"
#include "CIPv4Address.h"
class CPhysicalConnection
{
public:
	/**
	 *
	 * @param device the device must have a non-NULL ifa_addr
	 */
	CPhysicalConnection(struct ifaddrs* device);
	virtual ~CPhysicalConnection();
	void SetNetmask(int8_t maxNumberOfComputersInNetwork);
	void Receive(){mPacketCollector->ReceivePackets();}
	void GetConnectedDevicesIPAddresses();
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
	CMacAddress * mMacAddress;
	CIPv4Address * mIPAddress;
	struct ifreq mIfreq;
	CPacketCollector * mPacketCollector;
	set<CIPv4Address *> mConnectedDevicesIPv4Addresses;
	char * mInterfaceName;
};

#endif /* CPHYSICALCONNECTION_H_ */
