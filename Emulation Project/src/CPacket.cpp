/*
 * Packet.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: root
 */

#include "CPacket.h"

CPacket::CPacket(char * buffer,ssize_t & bufferSize):mSourceMacAddress(NULL),mDestinationMacAddress(NULL),mBuffer(NULL),mFrameSequenceCheck(0),mEthernetType(0)
{
	try
	{
		mBuffer = new CBuffer(buffer, bufferSize);
		mDestinationMacAddress = new CMacAddress(buffer, 0);
		mSourceMacAddress = new CMacAddress(buffer, ETH_ALEN);
		mEthernetType = uint16_t (buffer[ETH_ALEN * 2]);
		mFrameSequenceCheck = uint32_t(buffer+bufferSize-ETH_FCS_LEN);
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}
void CPacket::PrintLayerHead()
{
	try
	{
		cout<<"Ethernet- Printing Packet Ethernet Layer:\n";
		cout<<"Ethernet- Source Mac Address: \n";
		mSourceMacAddress->Print();
		cout<<"Ethernet- Destination Mac Address: \n";
		mDestinationMacAddress->Print();
		cout<<"Ethernet- Type: \n";
		PrintEthernetType();
		cout<<"Ethernet- Data:\n";
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}
void CPacket::PrintLayerTail()
{
	try
	{
		cout<<"Ethernet- FCS:\n";
		printf("%02X - IPv4 Type", mFrameSequenceCheck);
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}
void CPacket::PrintEthernetType()
{
	try
	{
		switch (mEthernetType)
		{
			case (ETH_P_IP):
				printf("%02X - IPv4 Type \n", mEthernetType);
				break;
			case (ETH_P_ARP):
				printf("%02X - ARP Type \n", mEthernetType);
				break;
			case (ETH_P_IPX):
				printf("%02X - IPX Type \n", mEthernetType);
				break;
			case (ETH_P_IPV6):
				printf("%02X - IPv6 Type \n", mEthernetType);
				break;
			default:
				printf("%02X - Unknown Type \n", mEthernetType);
				break;
		}
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}
CPacket::~CPacket()
{
	if (mBuffer!=NULL)
	{
		delete mBuffer;
		mBuffer=NULL;
	}
	if (mDestinationMacAddress!=NULL)
	{
		delete mDestinationMacAddress;
		mDestinationMacAddress=NULL;
	}
	if (mSourceMacAddress!=NULL)
	{
		delete mSourceMacAddress;
		mSourceMacAddress=NULL;
	}
}

