/*
 * CPhysicalConnection.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: project
 */

#include "CPhysicalConnection.h"

CPhysicalConnection::CPhysicalConnection(struct ifaddrs* device,int device_index)
{
	//read relevant data from stuct ifaddr and creates the appropriate
	//socket type?
	//bind the socket to one device exclusively
	struct ifreq ifr;
	struct sockaddr_ll soll;
	char buffer[1528]={0};

	try
	{
		mSocket = socket(device->ifa_addr->sa_family, SOCK_RAW | SOCK_NONBLOCK ,	0);

		if (mSocket == -1) {throw CException("socket is bad");}

		// Binding the socket to a specific device:
		memset(&ifr, 0, sizeof(ifr));
		strncpy(ifr.ifr_ifrn.ifrn_name,device->ifa_name, IFNAMSIZ);
		if (setsockopt(mSocket, SOL_SOCKET, SO_BINDTODEVICE, (void *) &ifr,
				sizeof(ifr)) < 0)
		{
			throw CException("binding of the socket to single device FAILED");
		}

		//add the device index to the binding operation
		memset(&soll,0,sizeof(soll));
		soll.sll_ifindex = device_index;
		soll.sll_protocol = 0;
		//TODO: how the fuck can we bind anything with sockaddr_ll when the bind methos uses sockaddr?
		//the manuel is lying to us!!!!
		if (bind(mSocket,(struct sockaddr*) &soll,sizeof(soll)) < 0)
		{
			throw CException("binding have failed");
		}

		ssize_t recvSize;
		// Command tries twice
		recvSize=recvfrom(mSocket,buffer,1528,MSG_DONTWAIT,NULL,0);
		if (recvSize == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
		{
			cerr << "err number " << errno << endl;
			throw CException("fatal error on recieve from socket");
		}

		cout << "packet: ";
		for (int i=0;i<1528;++i) {printf("%X",(unsigned int)buffer[i]);}
		cout << endl;

		close(mSocket);


	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
		perror("socket");
	}

}

CPhysicalConnection::~CPhysicalConnection()
{
	// TODO Auto-generated destructor stub
}

