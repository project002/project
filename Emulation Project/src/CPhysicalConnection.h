/*
 * CPhysicalConnection.h
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#ifndef CPHYSICALCONNECTION_H_
#define CPHYSICALCONNECTION_H_
#include "BasicIncludes.h"
#include "Protocols.h"
#include "CMacAddress.h"

class CPhysicalConnection
{
public:
	/**
	 *
	 * @param device the device must have a non-NULL ifa_addr
	 */
	CPhysicalConnection(struct ifaddrs* device);
	virtual ~CPhysicalConnection();
private:
	void InitStructs(struct ifaddrs* device);
	void ConfigureSocket(struct ifaddrs* device);
	void GetInterfaceInformation();
	int mSocket;
	int mInterfaceIndex;
	CMacAddress * mMacAddress;
	struct ifreq mIfreq;

	/**
	 * run a recv loop for a period of time specified
	 * @param period_len the duration of the rcv loop in seconds
	 */
	void Receive(int unsigned period_len); //run a loop to catch packets
	bool IsPacketEmpty(char* packet);
	/**
	 * outputs the input of the packet content which is a ipv4 packet
	 * @param ipHead a pointer to ipv4 packet inside the ethernet packet
	 */
	//void read_ipv4(char* ipHead);
};

#endif /* CPHYSICALCONNECTION_H_ */
