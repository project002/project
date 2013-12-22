/*
 * CMacAddress.h
 *
 *  Created on: Dec 17, 2013
 *      Author: root
 */

#ifndef CMACADDRESS_H_
#define CMACADDRESS_H_
#include "BasicIncludes.h"
enum EAddressType{
	EEmptyAddress=0,
	EBroadcastAddress=1
};

class CMacAddress
{
public:
	CMacAddress(EAddressType type)
	{
		int i=0;
		char ByteChar;
		if (type==EEmptyAddress)
		{
			ByteChar=0x0;
		}
		else
		{
			if (type==EBroadcastAddress)
			{
				ByteChar=0xff;
			}
		}
		for (; i < ETH_ALEN; i++)
		{
			mMacAddress[i]=ByteChar;
		}
	}
	CMacAddress()
	{
		int i;
		for (i = 0; i < ETH_ALEN; ++i)
		{
			mMacAddress[i] = 0;
		}
	}
	CMacAddress(char * AddressArr)
	{
		int i;
		for (i = 0; i < ETH_ALEN; ++i)
		{
			mMacAddress[i] = ((uint8_t) AddressArr[i]);
		}
	}

	CMacAddress(const CMacAddress &other)
	{
		int i=0;
		for(;i<ETH_ALEN;i++)
		{
			mMacAddress[i]=other.mMacAddress[i];
		}
	}
	/**
	 * creates a MAC address wrapper
	 * @param data a char array
	 * @param offset an offset in the given array that start with MAX address
	 */
	CMacAddress(char* data,int unsigned offset)
	{
		SetMAC(data,offset);
	}

	/**
	 * creates a MAC address wrapper
	 * @param data a char array
	 * @param offset an offset in the given array that start with MAX address
	 */
	void SetMAC(char* data,unsigned int offset)
	{
		unsigned int di,i;
		for (di=offset,i=0;di<offset+ETH_ALEN;++i,++di)
		{
			mMacAddress[i] = ((uint8_t) data[di]);
		}
	}

	string GetMacAddress()
	{
		stringstream ss;
		ss<<mMacAddress;
		return ss.str();
	}

	void Print()
	{
		for (int i=0;i<(ETH_ALEN-1);++i)
		{
			printf("%02X:",mMacAddress[i]);
		}
		printf("%02X\n",mMacAddress[ETH_ALEN-1]);
	}

	virtual ~CMacAddress()
	{
	}

private:
	unsigned char mMacAddress[ETH_ALEN];


};

#endif /* CMACADDRESS_H_ */
