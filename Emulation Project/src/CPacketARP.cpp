/*
 * CPacketARP.cpp
 *
 *  Created on: Dec 18, 2013
 *      Author: root
 */

#include "CPacketARP.h"

CPacketARP::CPacketARP(CMacAddress * MacSource, CIPv4Address *IPv4Source,CIPv4Address *IPv4Destination):
	CPacket(MacSource,new CMacAddress(EBroadcastAddress),ETH_P_ARP),
		mHardwareType(0x1), mProtocolType(0x800), mHardwareAddressLength(0x6), mProtocolAddressLength(
				0x4), mOpCode(0x1), mSourceMacAddress(NULL), mSourceIPAddress(NULL), mDestinationMacAddress(
				NULL), mDestinationIPAddress(NULL)
{
	try
	{
		((this->mSourceMacAddress))=MacSource;
		((this->mSourceIPAddress))=IPv4Source;
		((this->mDestinationMacAddress))= new CMacAddress(EEmptyAddress);
		((this->mDestinationIPAddress))=IPv4Destination;
		//call BUILD BUFFER from creator function
	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}
CPacketARP::CPacketARP(char * buffer, ssize_t &bufferSize) :
		CPacket(buffer, bufferSize), mHardwareType(0), mProtocolType(0), mHardwareAddressLength(
				0), mProtocolAddressLength(0), mOpCode(0),mSourceMacAddress(NULL),mSourceIPAddress(NULL),
				mDestinationMacAddress(NULL),mDestinationIPAddress(NULL)
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
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

CPacketARP::~CPacketARP()
{
	// TODO Auto-generated destructor stub
}

void CPacketARP::Print()
{
	try
	{
		CPacket::PrintLayerHead();
		PrintLayerHead();
		PrintLayerTail();
		CPacket::PrintLayerTail();
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

void CPacketARP::PrintLayerHead()
{
	try
	{
		LogColorChange(COLOR_RED);
		cout << "ARP- Printing Packet ARP Layer:\n";
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
		LogColorReset();
	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
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
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
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
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}
void CPacketARP::PrintOpCode()
{
	try
	{
		switch (mOpCode)
		{
			case (ARP_REQUEST_OP_CODE):
				printf("%02X - Arp Request \n", mOpCode);
				break;
			case (ARP_REPLY_OP_CODE):
				printf("%02X - Arp Reply \n", mOpCode);
				break;
			case (RARP_REQUEST_OP_CODE):
				printf("%02X - R-Arp Request \n", mOpCode);
				break;
			case (RARP_REPLY_OP_CODE):
				printf("%02X - R-Arp Reply \n", mOpCode);
				break;
			default:
				printf("%02X - Unknown Type \n", mOpCode);
				break;
		}
	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

void CPacketARP::PrintLayerTail()
{
	try
	{
		// No tail for ARP packets
	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

void CPacketARP::BuildBufferTail()
{
	try
	{
		CPacket::BuildBufferTail();
		//no tail
	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}
void CPacketARP::BuildBufferHeader()
{
	try
	{
		char buffer1Byte[1];
		char buffer2Bytes[2];
		CPacket::BuildBufferHeader();

		memcpy(buffer2Bytes, &mHardwareType, 2);
		(*mBuffer) += CBuffer(buffer2Bytes, 2);

		memcpy(buffer2Bytes, &mProtocolType, 2);
		(*mBuffer) += CBuffer(buffer2Bytes, 2);

		memcpy(buffer1Byte, &mHardwareAddressLength, 1);
		(*mBuffer) += CBuffer(buffer1Byte, 1);

		memcpy(buffer1Byte, &mProtocolAddressLength, 1);
		(*mBuffer) += CBuffer(buffer1Byte, 1);

		memcpy(buffer2Bytes, &mOpCode, 2);
		(*mBuffer) += CBuffer(buffer2Bytes, 2);

		(*mBuffer) += CBuffer(mSourceMacAddress->GetMacAddress().c_str(),
				ETH_ALEN);
		(*mBuffer) += CBuffer(mSourceIPAddress->GetIPv4Address().c_str(),
				IPv4_ALEN);
		(*mBuffer) += CBuffer(mDestinationMacAddress->GetMacAddress().c_str(),
				ETH_ALEN);
		(*mBuffer) += CBuffer(mDestinationIPAddress->GetIPv4Address().c_str(),IPv4_ALEN);
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}
void CPacketARP::BuildBuffer()
{
	try
	{
		BuildBufferHeader();
		BuildBufferTail();
	}
	catch(CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

/*
 *
 *
 * 	char * buffer=NULL;
	memcpy(buffer, &mFrameSequenceCheck, 4);
	(*mBuffer) += CBuffer(buffer, 4);


			mBuffer = new CBuffer(mSourceMacAddress->GetMacAddress().c_str(),ETH_ALEN);
		(*mBuffer) += CBuffer(mDestinationMacAddress->GetMacAddress().c_str(),ETH_ALEN);
		char * buffer=NULL;
		memcpy(buffer, &mEthernetType, 2);
		(*mBuffer) += CBuffer(buffer, 2);




	uint16_t mHardwareType; // Should be Ethernet = 0x1
	uint16_t mProtocolType; // Should be IP = 0x800
	uint8_t mHardwareAddressLength; // Should be MAC length = 0x6
	uint8_t mProtocolAddressLength; // Should be IPv4 length = 0x4 since IPv6 is returned by NDP protocol and not ARP
	uint16_t mOpCode; // When received it is a Request op code = 0x1
	CMacAddress *mSourceMacAddress;
	CIPv4Address *mSourceIPAddress;
	CMacAddress *mDestinationMacAddress;
	CIPv4Address *mDestinationIPAddress;
*/
