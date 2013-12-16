/*
 * CPhysicalConnection.h
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#ifndef CPHYSICALCONNECTION_H_
#define CPHYSICALCONNECTION_H_
#include "BasicIncludes.h"

#define ETH_FRAME_LEN 1518
#define IPV4ID 0x0800
#define ARPID  0x0806
#define IPXID  0x8137
#define IPV6ID 0x86DD

class CPhysicalConnection
{
public:
	/**
	 *
	 * @param device the device must have a non-NULL ifa_addr
	 */
	CPhysicalConnection(struct ifaddrs* device,int device_index);
	virtual ~CPhysicalConnection();
private:
	int mSocket;
	/**
	 * run a recv loop for a period of time specified
	 * @param period_len the duration of the rcv loop in seconds
	 */
	void run_recv_loop(int unsigned period_len); //run a loop to catch packets
	bool is_packet_empty(char* packet);
	/**
	 * outputs the input of the packet content which is a ipv4 packet
	 * @param ipHead a pointer to ipv4 packet inside the ethernet packet
	 */
	void read_ipv4(char* ipHead);
};

#endif /* CPHYSICALCONNECTION_H_ */
