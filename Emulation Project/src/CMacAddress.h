/*
 * CMacAddress.h
 *
 *  Created on: Dec 17, 2013
 *      Author: root
 */

#ifndef CMACADDRESS_H_
#define CMACADDRESS_H_
#include "BasicIncludes.h"
class CMacAddress
{
public:
	CMacAddress(char * AddressArr)
	{
		int i;
		for (i = 0; i < ETH_ALEN; ++i)
		{
			mMacAddress[i] = ((uint8_t) AddressArr[i]);
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
		cout << mMacAddress << endl;
	}
	virtual ~CMacAddress()
	{
	}

private:
	unsigned char mMacAddress[ETH_ALEN];
};

#endif /* CMACADDRESS_H_ */
