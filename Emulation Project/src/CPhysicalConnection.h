/*
 * CPhysicalConnection.h
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#ifndef CPHYSICALCONNECTION_H_
#define CPHYSICALCONNECTION_H_
#include "BasicIncludes.h"

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
};

#endif /* CPHYSICALCONNECTION_H_ */
