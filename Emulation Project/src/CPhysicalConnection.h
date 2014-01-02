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

#define MAX_NUMBER_OF_COMPUTERS_ON_NET 32

class CPhysicalConnection:public CConnection
{
public:
	/**
	 *
	 * @param device the device must have a non-NULL ifa_addr
	 */

	CPhysicalConnection(struct ifaddrs* device);
	virtual ~CPhysicalConnection();
	void SetNetmask(CUIPV4* getway_addr,CUIPV4* mIPMaskAddress);
	void Receive(){mPacketCollector->ReceivePackets();}
	virtual vector<string>& GetTable()const;
	/**
	 * starts DHCP service om the current interface
	 */
	void startDHCPService();

	const char* getInterfaceName() const
	{
		return mInterfaceName;
	}

private:
	void InitStructs(struct ifaddrs* device);
	void ConfigureSocket(struct ifaddrs* device);
	void GetInterfaceInformation();
	void setMaskAddress();
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

	int mInterfaceIndex;
	CUIPV4* mIPMaskAddress;
	CUIPV4* mGetwayAddress;
	struct ifreq mIfreq;
	CPacketCollector * mPacketCollector;
	map<string,string> mConnectedDevicesIPv4Addresses;
	char * mInterfaceName;

	//DHCPServices for Physical Connection
	CDHCPService* mDHCPsrv;

	Sniffer* mDHCPSniffer;
	Sniffer* mIPSniffer;


};

#endif /* CPHYSICALCONNECTION_H_ */
