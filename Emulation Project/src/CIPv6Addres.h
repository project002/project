/*
 * CIPv6Addres.h
 *
 *  Created on: Dec 18, 2013
 *      Author: root
 */

#ifndef CIPV6ADDRES_H_
#define CIPV6ADDRES_H_


#include "BasicIncludes.h"
#define IPv6_ALEN 16
class CIPv6Address
{
public:
	CIPv6Address()
	{
		unsigned int i;
		for (i=0;i<IPv6_ALEN;++i)
		{
			mIPAddress[i] = 0 ;
		}
	}
	CIPv6Address(char* data,int unsigned offset)
	{
		unsigned int di,i;
		for (di=offset,i=0;di<offset+IPv6_ALEN;++i,++di)
		{
			mIPAddress[i] = ((uint8_t) data[di]);
		}
	}
	virtual ~CIPv6Address()
	{

	}
	void Print()
	{
		int i;
		for ( i=0;i<(IPv6_ALEN-1);++i)
		{
			printf("%u:",uint16_t(mIPAddress[i]));
		}
		printf("%u",uint16_t(mIPAddress[i]));
		cout<< endl;
		for ( i=0;i<(IPv6_ALEN-2);i+=2)
		{
			printf("%02X",uint16_t(mIPAddress[i]));
			printf("%02X:",uint16_t(mIPAddress[i+1]));
		}
		printf("%02X",uint16_t(mIPAddress[i]));
		printf("%02X",uint16_t(mIPAddress[i+1]));
		cout<< endl;
		printf("%u\n",uint16_t(mIPAddress[IPv6_ALEN-1]));
	}

	/**
	 * creates a IP address wrapper
	 * @param data a char array
	 * @param offset an offset in the given array that start with MAX address
	 */
	void SetIP(char* data,unsigned int offset)
	{
		unsigned int di,i;
		for (di=offset,i=0;di<offset+IPv6_ALEN;++i,++di)
		{
			mIPAddress[i] = ((uint8_t) data[di]);
		}
	}
private:
	unsigned char mIPAddress[IPv6_ALEN];
};

#endif /* CIPV6ADDRES_H_ */
