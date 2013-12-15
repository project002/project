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
//	struct sockaddr_ll socket_address;


	try
	{
		mSocket = socket(device->ifa_addr->sa_family, SOCK_RAW | SOCK_NONBLOCK , htons(ETH_P_ALL));

		if (mSocket == -1) {throw CException("socket is bad");}

		// Binding the socket to a specific device:
		memset(&ifr, 0, sizeof(ifr));
		strncpy(ifr.ifr_ifrn.ifrn_name,device->ifa_name, IFNAMSIZ);
		if (setsockopt(mSocket, SOL_SOCKET, SO_BINDTODEVICE, (void *) &ifr,
				sizeof(ifr)) < 0)
		{
			throw CException("binding of the socket to single device FAILED");
		}

		//read loop
		run_recv_loop(30);

	}
	catch (CException & e)
	{
		std::cerr << e.what() << std::endl;
		perror("socket");
		close(mSocket);
	}

}

CPhysicalConnection::~CPhysicalConnection()
{
	// TODO Auto-generated destructor stub
	close(mSocket);
}

bool CPhysicalConnection::is_packet_empty(char* buffer)
{
	int sum = 0;
	for (int i=0;i<ETH_FRAME_LEN;++i)
	{
		sum += buffer[i];
		if (sum>0) {return false;}
	}
	return true;
}

void CPhysicalConnection::run_recv_loop(int unsigned period_len)
{
	time_t start = time(NULL);
	int len = (int) period_len;
	char buffer[ETH_FRAME_LEN]={0};
	ssize_t recvSize;
	int dMAC_range[2] = {0,ETH_ALEN-1};
	int sMAC_range[2] = {ETH_ALEN,(2*ETH_ALEN)-1};
	while (time(NULL) < start+len)
	{
		try
		{
			recvSize=recvfrom(mSocket,buffer,ETH_FRAME_LEN,0,NULL,NULL);
			if (recvSize == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
			{
				cerr << "err number " << errno << endl;
				throw CException("fatal error on receive from socket");
			}

			if (!is_packet_empty(buffer)) //don't print empty packets
			{
				cout << "packet: " << endl;
				cout << "\t Dest MAC: ";
				for (int dm=0;dm<ETH_ALEN;++dm) {printf("%02X:",buffer[dm]);}
				cout << endl;

				cout << "\t Src MAC:";
				for (int sm=0;sm<ETH_ALEN;++sm) {printf("%02X:",buffer[ETH_ALEN+sm]);}
				cout << endl;

				cout << "\t Type ID:";
				printf("%02X",buffer[ETH_ALEN*2]);
				printf("%02X",buffer[(ETH_ALEN*2)+1]);
				cout << endl;

				cout << "\t RAW: ";
				for (int i=0;i<ETH_FRAME_LEN;++i)
				{
					printf("%02X",buffer[i]);
					buffer[i] = 0; //re-initialize the buffer
				}
				cout << endl;
			}
		}
		catch (CException & e)
		{
			std::cerr << e.what() << std::endl;
			perror("socket");
			break;
		}
	}
}

