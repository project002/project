/*
 * CPacketIPv6.cpp
 *
 *  Created on: Dec 18, 2013
 *      Author: root
 */

#include "CPacketIPv6.h"

CPacketIPv6::CPacketIPv6(char * buffer, ssize_t &bufferSize) :
		CPacket(buffer, bufferSize), mVersion(0), mTrafficClass(0), mFlowLabel(
				0), mPayloadLength(0), mNextHeader(0), mHopLimit(0), mSourceIPAddress(
				NULL), mDestinationIPAddress(NULL)
{
	try
		{
			uint32_t calculator=0;
			unsigned int currentOffset = CPacket::GetHeadSize();
			mVersion = (buffer[currentOffset] & 0xf0 )>>4;
			calculator =(buffer[currentOffset] & 0xf) <<4;
			currentOffset+= 1;
			calculator |= (buffer[currentOffset]>>4 &0xf );
			mTrafficClass = calculator;
			calculator=0;
			calculator=(buffer[currentOffset] & 0xf) <<16;
			currentOffset+= sizeof(uint8_t);
			calculator |= (buffer[currentOffset] &0xff )<<8;
			currentOffset+= sizeof(uint8_t);
			calculator |= (buffer[currentOffset] &0xff );
			currentOffset+= sizeof(uint8_t);
			mFlowLabel =calculator;

			mPayloadLength = ( (buffer[currentOffset] & 0xFF) << 8 ) | (buffer[currentOffset+1] & 0xFF);
			currentOffset+= sizeof(uint16_t);

			mNextHeader= buffer[currentOffset];
			currentOffset+= sizeof(uint8_t);

			mHopLimit = buffer[currentOffset];
			currentOffset+= sizeof(uint8_t);

			mSourceIPAddress = new CIPv6Address(buffer,currentOffset);
			currentOffset+= IPv6_ALEN;

			mDestinationIPAddress = new CIPv6Address(buffer,currentOffset);
			currentOffset+= IPv6_ALEN;
		}
		catch (CException & e)
		{
			std::cerr << e.what() << std::endl;

		}
}

CPacketIPv6::~CPacketIPv6()
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

void CPacketIPv6::Print()
{
	CPacket::PrintLayerHead();
	PrintLayerHead();
	PrintLayerTail();
	CPacket::PrintLayerTail();
}

void CPacketIPv6::PrintLayerHead()
{
	try
	{
		LogColorChange(COLOR_RED);
		cout << "IPv6- Printing Packet IPv6 Layer :\n";
		cout << "IPv6- Version :\n";
		printf("%02X \n", mVersion);

		cout << "IPv6- Traffic Class :\n";
		printf("%02X \n", mTrafficClass);

		cout << "IPv6- Flow Label :\n";
		printf("%02X \n", mFlowLabel);

		cout << "IPv6- Payload Length :\n";
		printf("%02X \n", mPayloadLength);

		cout << "IPv6- Next Header :\n";
		printf("%02X \n", mNextHeader);

		cout << "IPv6- Hop Limit :\n";
		printf("%02X \n", mHopLimit);

		cout << "IPv6-Source IP Address :\n";
		mSourceIPAddress->Print();

		cout << "IPv6-Destination IP Address :\n";
		mDestinationIPAddress->Print();
		LogColorReset();
	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
	}
}
void CPacketIPv6::PrintLayerTail()
{
	try
	{

	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;

	}
}
