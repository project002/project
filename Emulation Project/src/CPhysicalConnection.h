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
	virtual vector<string>& GetTable()const;
	virtual Crafter::Packet* GetPacket();
	virtual bool SendPacket(Packet* packet) const;
	/**
	 * starts DHCP service om the current interface
	 */
	void startDHCPService();

	const char* getInterfaceName() const{return mInterfaceName;}

	CUIPV4*& getGetwayAddress()  {return mGetwayAddress;}

	void setGetwayAddress( CUIPV4*& getwayAddress){mGetwayAddress = getwayAddress;}

private:
	void InitStructs(struct ifaddrs* device);
	void ConfigureSocket(struct ifaddrs* device);
	void GetInterfaceInformation();
	void setMaskAddress();

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
