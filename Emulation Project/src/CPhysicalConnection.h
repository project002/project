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
};

#endif /* CPHYSICALCONNECTION_H_ */
