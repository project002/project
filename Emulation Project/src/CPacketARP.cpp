/*
 * CPacketARP.cpp
 *
 *  Created on: Dec 18, 2013
 *      Author: root
 */

#include "CPacketARP.h"

CPacketARP::CPacketARP(char * buffer, ssize_t &bufferSize) :
		CPacket(buffer, bufferSize), mHardwareType(0), mProtocolType(0), mHardwareAddressLength(
				0), mProtocolAddressLength(0), mOpCode(0)
{
	try
	{
		unsigned int currentOffset = CPacket::GetHeadSize();
		mHardwareType = buffer[currentOffset]<<8 | buffer[currentOffset+1];
		currentOffset += sizeof(uint16_t);
		mProtocolType = buffer[currentOffset]<<8 | buffer[currentOffset+1];
		currentOffset += sizeof(uint16_t);
		mHardwareAddressLength = uint8_t(buffer[currentOffset]);
		currentOffset += sizeof(uint8_t);
		mProtocolAddressLength = uint8_t(buffer[currentOffset]);
		currentOffset += sizeof(uint8_t);
		mOpCode = buffer[currentOffset]<<8 | buffer[currentOffset+1];
		currentOffset += sizeof(uint16_t);
		mSourceMacAddress= new CMacAddress(buffer, currentOffset);
		currentOffset += ETH_ALEN;
		mSourceIPAddress=new CIPv4Address(buffer, currentOffset);
		currentOffset += IPv4_ALEN;
		mDestinationMacAddress= new CMacAddress(buffer, currentOffset);
		currentOffset += ETH_ALEN;
		mDestinationIPAddress=new CIPv4Address(buffer, currentOffset);
		currentOffset += IPv4_ALEN;

	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;

	}
}

CPacketARP::~CPacketARP()
{
	// TODO Auto-generated destructor stub
}

void CPacketARP::Print()
{
	CPacket::PrintLayerHead();
	PrintLayerHead();
	PrintLayerTail();
	CPacket::PrintLayerTail();
}

void CPacketARP::PrintLayerHead()
{
	try
	{
		string HeadLog ="ARP- Printing Packet ARP Layer:\n";
		LogColor(HeadLog.c_str(),COLOR_BLUE);
		cout << "ARP- Hardware Type:\n";
		PrintHardwareType();
		cout << "ARP- Protocol Type:\n";
		PrintProtocolType();
		cout << "ARP- Hardware Address Length:\n";
		printf("%02X \n", mHardwareAddressLength);
		cout << "ARP- Protocol Address Length:\n";
		printf("%02X \n", mProtocolAddressLength);
		cout << "ARP- Op Code :\n";
		PrintOpCode();
		cout << "ARP-Source Mac Address :\n";
		mSourceMacAddress->Print();
		cout << "ARP-Source IP Address :\n";
		mSourceIPAddress -> Print();
		cout << "ARP-Destination Mac Address :\n";
		mDestinationMacAddress -> Print();
		cout << "ARP-Destination IP Address :\n";
		mDestinationIPAddress -> Print();
		cout << "ARP- Destination Mac Address: ";
		mDestinationMacAddress->Print();
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void CPacketARP::PrintProtocolType()
{
	try
	{
		switch (mProtocolType)
		{
			case (ARP_IP_PROTOCOL):
				printf("%02X - IPv4 \n", mProtocolType);
				break;
			default:
				printf("%02X - Unknown Type \n", mProtocolType);
				break;
		}
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}
void CPacketARP::PrintHardwareType()
{
	try
	{
		switch (mHardwareType)
		{
			case (ARP_ETHERNET_PROTOCOL):
				printf("%02X - Ethernet \n", mHardwareType);
				break;
			default:
				printf("%02X - Unknown Type \n", mHardwareType);
				break;
		}
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}
void CPacketARP::PrintOpCode()
{
	try
	{
		switch (mOpCode)
		{
			case (ARP_REQUEST_OP_CODE):
				printf("%02X - Request \n", mOpCode);
				break;
			default:
				printf("%02X - Unknown Type(probably reply) \n", mOpCode);
				break;
		}
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void CPacketARP::PrintLayerTail()
{
	try
	{
		// No tail for ARP packets
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}
