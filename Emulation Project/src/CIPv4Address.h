/*
 * CIPv4Address.h
 *
 *  Created on: Dec 18, 2013
 *      Author: root
 */

#ifndef CIPV4ADDRESS_H_
#define CIPV4ADDRESS_H_
#include "BasicIncludes.h"

class CIPv4Address
{
public:
	CIPv4Address()
	{
		unsigned int i;
		for (i=0;i<IPv4_ALEN;++i)
		{
			mIPAddress[i] = 0 ;
		}
	}
	CIPv4Address(char* data,int unsigned offset)
	{
		unsigned int di,i;
		for (di=offset,i=0;di<offset+IPv4_ALEN;++i,++di)
		{
			mIPAddress[i] = ((uint8_t) data[di]);
		}
	}
	virtual ~CIPv4Address()
	{

	}
	void Print()
	{
		for (int i=0;i<(IPv4_ALEN-1);++i)
		{
			printf("%u:",uint16_t(mIPAddress[i]));
		}
		printf("%u\n",uint16_t(mIPAddress[IPv4_ALEN-1]));
	}

	/**
	 * creates a IP address wrapper
	 * @param data a char array
	 * @param offset an offset in the given array that start with MAX address
	 */
	void SetIP(char* data,unsigned int offset)
	{
		unsigned int di,i;
		for (di=offset,i=0;di<offset+IPv4_ALEN;++i,++di)
		{
			mIPAddress[i] = ((uint8_t) data[di]);
		}
	}
private:
	unsigned char mIPAddress[IPv4_ALEN];
};

#endif /* CIPV4ADDRESS_H_ */
