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
		mEthernetType =((buffer[ETH_ALEN * 2] & 0xFF) << 8) | (buffer[(ETH_ALEN * 2) + 1] & 0xFF) ;
		mFrameSequenceCheck = uint32_t(buffer+bufferSize-ETH_FCS_LEN);
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

CPacket::CPacket(CMacAddress * SourceMacAddress,
		CMacAddress * DestinationMacAddress, uint16_t EthernetType) :
		mSourceMacAddress(SourceMacAddress), mDestinationMacAddress(
				DestinationMacAddress), mBuffer(NULL), mFrameSequenceCheck(0), mEthernetType(
				EthernetType)
{
	try
	{
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}
void CPacket::PrintLayerHead()
{
	try
	{
		LogColorChange(COLOR_BLUE);
		cout << "Ethernet- Printing Packet Ethernet Layer:\n";
		cout << "Ethernet- Source Mac Address: \n";
		mDestinationMacAddress ->Print();
		cout << "Ethernet- Destination Mac Address: \n";
		mSourceMacAddress->Print();
		cout << "Ethernet- Type: \n";
		PrintEthernetType();
		cout << "Ethernet- Data:\n";
		LogColorReset();
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}
void CPacket::PrintLayerTail()
{
	try
	{
		LogColorChange(COLOR_BLUE);
		cout<<"Ethernet- FCS:\n";
		printf("%02X \n", mFrameSequenceCheck);
		LogColorReset();
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
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
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}
CPacket::~CPacket()
{
	try
	{
		if (mBuffer != NULL)
		{
			delete mBuffer;
			mBuffer = NULL;
		}
		if (mDestinationMacAddress != NULL)
		{
			delete mDestinationMacAddress;
			mDestinationMacAddress = NULL;
		}
		if (mSourceMacAddress != NULL)
		{
			delete mSourceMacAddress;
			mSourceMacAddress = NULL;
		}
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

void CPacket::BuildBuffer()
{
	try
	{

	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}
void CPacket::BuildBufferTail()
{
	char buffer[4];
	memcpy(buffer, &mFrameSequenceCheck, 4);
	(*mBuffer) += CBuffer(buffer, 4);
}
void CPacket::BuildBufferHeader()
{
	try
	{
		mBuffer = new CBuffer(mSourceMacAddress->GetMacAddress().c_str(),ETH_ALEN);
		(*mBuffer) += CBuffer(mDestinationMacAddress->GetMacAddress().c_str(),ETH_ALEN);
		char  buffer[2];
		memcpy(buffer, &mEthernetType, 2);
		(*mBuffer) += CBuffer(buffer, 2);
		//FIXME : endian is messed up! it copies the 2 byte parameters reveresed.
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}
