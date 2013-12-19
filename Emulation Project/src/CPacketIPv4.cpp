/*
 * CPacketIPv4.cpp
 *
 *  Created on: Dec 18, 2013
 *      Author: root
 */

#include "CPacketIPv4.h"

CPacketIPv4::CPacketIPv4(char * buffer, ssize_t &bufferSize) :
		CPacket(buffer, bufferSize), mVersion(0), mHeaderLength(0),

		mServiceType(0), mTotalLength(0), mIdentification(0), mFlags(0), mFragmentationOffset(
				0), mTimeToLive(0), mProtocol(0), mHeaderChecksum(0), mSourceIPAddress(
				NULL), mDestinationIPAddress(NULL), mOptions(0)
{
	try
	{
		srand(time(NULL));
		unsigned int currentOffset = CPacket::GetHeadSize();
		mVersion = buffer[currentOffset]>>4;
		mHeaderLength = buffer[currentOffset] & 0xf;
		currentOffset+= sizeof(uint8_t);
		mServiceType = buffer[currentOffset];
		currentOffset+= sizeof(uint8_t);
		mTotalLength = buffer[currentOffset] << 8 | buffer[currentOffset+1];
		currentOffset+= sizeof(uint16_t);
		mIdentification = buffer[currentOffset] << 8 | buffer[currentOffset+1];
		currentOffset+= sizeof(uint16_t);
		mFlags = buffer[currentOffset] >>5;
		mFragmentationOffset = buffer[currentOffset] & 0x1f;
		currentOffset+= sizeof(uint8_t);
		mTimeToLive = buffer[currentOffset];
		currentOffset+= sizeof(uint8_t);
		mProtocol = buffer[currentOffset];
		currentOffset+= sizeof(uint8_t);
		mHeaderChecksum = buffer[currentOffset] << 8 | buffer[currentOffset+1];
		currentOffset+= sizeof(uint16_t);
		mSourceIPAddress = new CIPv4Address(buffer,currentOffset);
		currentOffset+= IPv4_ALEN;
		mDestinationIPAddress = new CIPv4Address(buffer,currentOffset);
		currentOffset+= IPv4_ALEN;

		int i=mHeaderLength;
		for (;i>5;i--)
		{
			cout<< "\n LOOP " << i << "\n";
			int j=0;
			for (;j<4;j++)
			{
				mOptions += buffer[currentOffset];
				currentOffset+=sizeof(uint8_t);
				mOptions = mOptions << 8;
			}
		}

	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;

	}
}
void CPacketIPv4::PrintLayerHead()
{
	try
	{
		LogColorChange(COLOR_RED);

		cout <<"IPv4- Printing Packet IPv4 Layer :\n";
		cout << "IPv4- Version :\n";
		printf("%02X \n", mVersion);

		cout << "IPv4- Header Length :\n";
		printf("%02X \n", mHeaderLength);

		cout << "IPv4- Service Type :\n";
		printf("%02X \n", mServiceType);

		cout << "IPv4- Total Length :\n";
		printf("%02X \n", mTotalLength);

		cout << "IPv4- Identification :\n";
		printf("%02X \n", mIdentification);

		cout << "IPv4- Flags :\n";
		printf("%02X \n", mFlags);

		cout << "IPv4- Fragment Offset :\n";
		printf("%02X \n", mFragmentationOffset);

		cout << "IPv4- Time To live :\n";
		printf("%02X \n", mTimeToLive);

		cout << "IPv4- Protocol :\n";
		printf("%02X \n", mProtocol);

		cout << "IPv4- Checksum :\n";
		printf("%02X \n", mHeaderChecksum);

		cout << "IPv4-Source IP Address :\n";
		mSourceIPAddress -> Print();

		cout << "IPv4-Destination IP Address :\n";
		mDestinationIPAddress -> Print();

		cout << "IPv4-Options :\n";
		printf("%016llX \n", mOptions);

		LogColorReset();
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}
void CPacketIPv4::PrintLayerTail()
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

CPacketIPv4::~CPacketIPv4()
{
	if (mSourceIPAddress!=NULL)
	{
		delete mSourceIPAddress;
		mSourceIPAddress=NULL;
	}
	if (mDestinationIPAddress!=NULL)
	{
		delete mDestinationIPAddress;
		mDestinationIPAddress=NULL;
	}
}

void CPacketIPv4::Print()
{
	CPacket::PrintLayerHead();
	PrintLayerHead();
	PrintLayerTail();
	CPacket::PrintLayerTail();
}

