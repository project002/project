/*
 * CPhysicalConnectionsHandler.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#include "CPhysicalConnectionsHandler.h"
#define STATUS_FAILURE -1
#define ERROR_MSG_GETTING_DEVICE_LIST "Err: when retrieving devices list"

/**
 * Class C-tor - Nothing to do so cleaning the vector.
 */
CPhysicalConnectionsHandler::CPhysicalConnectionsHandler()
{
	try
	{
		mPhysicalConnections.clear();
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

/**
 * Class D-tor - releasing all allocated physical connection.
 * All of them should be freed only here!
 */
CPhysicalConnectionsHandler::~CPhysicalConnectionsHandler()
{
	try
	{
		vector<CPhysicalConnection * >::iterator it;
		for (it = mPhysicalConnections.begin(); it!=mPhysicalConnections.end();it++)
		{
			if ((*it)!=NULL)
			{
				delete (*it);
				(*it)=NULL;
			}

		}
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

/**
 * Creating the physical functions
 */
void CPhysicalConnectionsHandler::CreatePhyiscalConnections()
{
	try
	{
		struct ifaddrs* list;
		struct ifaddrs* node;
		int ret = 0;

		//get devices list
		ret = getifaddrs(&list);
		if (ret == STATUS_FAILURE) {throw CException(ERROR_MSG_GETTING_DEVICE_LIST);}
		CPhysicalConnection * newConnection=NULL;

		//iterate over list
		int i = 0;
		for (i=0,node = list; node != NULL; node = node->ifa_next,++i)
		{
			std::cout << "name:\t" << node->ifa_name << std::endl;

			if (node->ifa_addr != NULL && node->ifa_addr->sa_family == AF_PACKET && strcmp(node->ifa_name, "lo")  )
			{
				newConnection = new CPhysicalConnection(node);
				mPhysicalConnections.push_back(newConnection);
			}
		}

		//free the device list
		freeifaddrs(list);
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}

/**
 * Returning a physical connection pointer on demand, using its name to be added to the
 * router using it.
 *
 * @param InterfaceName
 * @return physical connection pointer
 */
 CPhysicalConnection const * CPhysicalConnectionsHandler::GetPhysicalConnectionByName(
		const char * InterfaceName)
{
	try
	{
		vector<CPhysicalConnection *>::iterator it;
		for (it = mPhysicalConnections.begin();it!=mPhysicalConnections.end();it++)
		{
			const char * PhysicalInterfaceName=(*it)->getInterfaceName();
			if(!strcmp(PhysicalInterfaceName, InterfaceName))
			{
				return (*it);
			}
		}
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;

	}
	return NULL;
}































































/**
 * Function performs system calls to check the connected Ethernet cards and open sockets accordingly
 * using the Physical Connection class
 */
/*void CPhysicalConnectionsHandler::CreatePhyiscalConnections()
{
	try
	{
		struct ifaddrs* list;
		struct ifaddrs* node;
		int ret = 0;
		int address_size = 0;
		int address_int = 0;
		char host_buffer[NI_MAXHOST];

		//get devices list
		ret = getifaddrs(&list);
		if (ret == -1) {throw CException("Err: when retrieving devices list");}

		//iterate over list
		for (node = list; node != NULL; node = node->ifa_next)
		{
			std::cout << "name:\t" << node->ifa_name << std::endl;

			if (node->ifa_addr == NULL)
			{
				std::cout << "empty address" << std::endl;
				continue;
			}

			std::cout << "\tfamily type:";
			switch (node->ifa_addr->sa_family)
			{
				case AF_PACKET:
					std::cout << "AF_PACKET";
					break;
				case AF_INET:
					std::cout << "AF_INET";
					break;
				case AF_INET6:
					std::cout << "AF_INET6";
					break;
			}
			std::cout << std::endl;

			std::cout << "\taddress:";
			switch (node->ifa_addr->sa_family)
			{
				case AF_INET:
					address_size = sizeof(struct sockaddr_in);
					break;
				case AF_INET6:
					address_size = sizeof(struct sockaddr_in6);
					break;
			}
			address_int = getnameinfo(node->ifa_addr, address_size, host_buffer,
					NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

			if (address_int != 0)
			{
				std::cout << "no address" << std::endl;
				continue;
			}
			else
			{
				std::cout << host_buffer << std::endl;
			}

		}

		//clear the allocated list
		freeifaddrs(list);
	}
	catch (CException & error)
	{
		std::cerr << error.what() << std::endl;
		std::cerr << __PRETTY_FUNCTION__ << std::endl;
	}
}*/

