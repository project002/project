/*
 * H_SNIFFER_FUNC.h
 *
 *  Created on: Dec 26, 2013
 *      Author: root
 */

#ifndef H_SNIFFER_FUNC_H_
#define H_SNIFFER_FUNC_H_
#include <crafter.h>
#include "CDHCPService.h"
/***************************************************
 * will contain all the methods that are actually
 * wrappers for the packet sniffers methods
 **************************************************/


/**
 * runs the DHCP service given
 * @param p Packet form an interface
 * @param DHCPService a CDHCPService object type
 */
inline void runDHCPService(Packet* p,void* DHCPService)
{
	CDHCPService* srv = (static_cast<CDHCPService*>(DHCPService));
	srv->start(p);
}


#endif /* H_SNIFFER_FUNC_H_ */
