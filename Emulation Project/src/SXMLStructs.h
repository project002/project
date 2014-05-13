/*
 * SXMLStructs.h
 *
 *  Created on: Mar 11, 2014
 *      Author: root
 */

#ifndef SXMLSTRUCTS_H_
#define SXMLSTRUCTS_H_


struct RouterInformation
{
	RouterInformation()
	{
		sRouterNumber=0;
		sBufferSize=1024;
		sDropRate=0;
		sUsedBufferSize=0;
		sFillage=0;
	}
	unsigned int sRouterNumber;
	unsigned int sBufferSize;
	double sDropRate;
	unsigned int sUsedBufferSize;
	double sFillage;
};


#endif /* SXMLSTRUCTS_H_ */
