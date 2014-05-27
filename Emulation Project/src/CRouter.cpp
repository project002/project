/*
 * CRouter.cpp
 *
 *  Created on: Dec 31, 2013
 *      Author: root
 */

#include "CRouter.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include "CPhysicalConnection.h"

CRouter::CRouter():mBufferSize(DEFAULT_ROUTER_BUFFER_SIZE),mDropRate(0),mFillage(0),mInitialBufferUse(0),mRouterNumber(1024),mPacketCollector(NULL),mThreaded(true)
{
	try
	{

	}
	catch(CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}

}

void CRouter::AppendConnectionList(list<CVirtualConnection const *> &connectionList)
{
	try
	{
		if (connectionList.empty())
		{
			return;
		}
		for (list<CVirtualConnection const *>::iterator iter= connectionList.begin();
				iter!=connectionList.end();iter++)
		{
			mConnections.push_back((*iter));
			CVirtualConnection * virtualRef=const_cast<CVirtualConnection*>((*iter));
			virtualRef->AddRoutingTableReference(&mRoutingTable,mRouterNumber);
		}
	}
	catch(CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}
void CRouter::RequestTables()
{
	try
	{
		list<CConnection const *>::iterator iter;
		//iterate over all connections
		stringstream s;
		for(iter = mConnections.begin();iter!=mConnections.end();iter++)
		{
			//s <<"Print Routing Table For " << (*iter)->GetMAC() << endl;
			//iterate over all ips in the table you got from the connection
			vector< pair<string,string> >& tables=const_cast<CConnection*>(*iter)->GetTable();
			//print table
			vector< pair<string,string> >::iterator it=tables.begin();
			for (;it!=tables.end();it++)
			{
				mRoutingTable.insert(pair<string,pair<CConnection const*,string> >(
						it->first,
						pair<CConnection const*,string>((*iter),it->second)
					));
			}
		}
	}
	catch(CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

void CRouter::Sniffer()
{
	try
	{
		mDropRateArrIterator=mDropRateArr.begin();
		mPacketCollector= new CPacketCollector(mBufferSize);
		AddPacketsToBuffer(mInitialBufferUse);
		mSniffingThread = boost::thread(&CRouter::Sniff, this);
		mPacketsHandlingThread = boost::thread(&CRouter::PacketHandler, this);
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

void CRouter::nonThreadedInit()
{
	try
	{
		mPacketCollector= new CPacketCollector(mBufferSize);
		mDropRateArrIterator=mDropRateArr.begin();
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

void CRouter::nonThreadedSniffer()
{
	try
	{
		mThreaded = false;
		Sniff();
		PacketHandler();
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}


bool CRouter::ProcessSendPacket(Packet* packet)
{
	map<string,pair< CConnection const*,string> >::iterator table_pos;
	IP* ip_layer = packet->GetLayer<IP>();
	Ethernet* eth_layer = packet->GetLayer<Ethernet>();

	table_pos = mRoutingTable.find(ip_layer->GetDestinationIP());

	string DestMAC = table_pos->second.second;
	string SrcMAC = table_pos->second.first->GetMAC();

	eth_layer->SetSourceMAC(SrcMAC);
	eth_layer->SetDestinationMAC(DestMAC);

	ip_layer->SetTTL(ip_layer->GetTTL()-1);
	return (ip_layer->GetTTL() >= 1);
}

void CRouter::DynamicFillageHandler()
{
	try
	{
		static double previousChangeTime = SReport::getInstance().GetReportElapsedTime();
		double elapsedTime=SReport::getInstance().GetReportElapsedTime() - previousChangeTime;
		if ((*mFillageArrIterator).second < elapsedTime)
		{
			mFillageArrIterator++;
			if (mFillageArrIterator==mFillageArr.end())
			{
				mFillageArrIterator = mFillageArr.begin();
			}
			SetFillage((*mFillageArrIterator).first);
			previousChangeTime = SReport::getInstance().GetReportElapsedTime();
		}
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}
void CRouter::DynamicDropRateHandler()
{
	try
	{
		static double previousChangeTime = SReport::getInstance().GetReportElapsedTime();
		double elapsedTime=SReport::getInstance().GetReportElapsedTime() - previousChangeTime;
		if ((*mDropRateArrIterator).second < elapsedTime)
		{
			mDropRateArrIterator++;
			if (mDropRateArrIterator==mDropRateArr.end())
			{
				mDropRateArrIterator = mDropRateArr.begin();
			}
			SetDropRate((*mDropRateArrIterator).first);
			previousChangeTime = SReport::getInstance().GetReportElapsedTime();
		}
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

void CRouter::PacketHandler()
{
	Packet* packet;
	map<string,pair< CConnection const*,string> >::iterator pos;
	map<string,pair< CConnection const*,string> >::iterator con_pos;
	mFillageArrIterator = mFillageArr.begin();
	double popTime=0;
	try
	{
		while (true)
		{
			boost::this_thread::interruption_point();
			if(!mFillageArr.empty())
			{
				DynamicFillageHandler();
			}
			mPacketCollector->FixBufferFillage(mFillage);
			packet = mPacketCollector->PopFront(popTime);
			if(packet!=NULL)
			{
				IP* ip_layer = packet->GetLayer<IP>();
				if (ip_layer != NULL)
				{
					SDataController::getInstance().incData(SDataController::IPPACKET);
					HandleIPv4(packet,popTime);
				}
				else
				{
					ARP* arp_layer = packet->GetLayer<ARP>();
					if (arp_layer != NULL) //answer arp requests
					{
						SDataController::getInstance().incData(SDataController::ARPPACKET);
						HandleArp(packet);
					}
				}
				delete packet;
			}
			if (!mThreaded) {break;} //loop once when not threaded
		}
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

void CRouter::HandleArp(Packet * pkt)
{
	CConnection* send_connection;
	map<string,pair< CConnection const*,string> >::iterator pos;
	map<string,pair< CConnection const*,string> >::iterator con_pos;
	ARP* arp_layer = pkt->GetLayer<ARP>();
	con_pos = mRoutingTable.find(arp_layer->GetSenderIP());
	pos = mRoutingTable.find(arp_layer->GetTargetIP());
	if (pos != mRoutingTable.end() && con_pos != mRoutingTable.end())
	{
		Ethernet* eth_layer = pkt->GetLayer<Ethernet>();
		if (arp_layer->GetOperation() == ARP::Request && eth_layer != NULL)
		{
			arp_layer->SetOperation(ARP::Reply);
			eth_layer->SetDestinationMAC(arp_layer->GetSenderMAC());
			arp_layer->SetTargetMAC(arp_layer->GetSenderMAC());
			arp_layer->SetSenderMAC(con_pos->second.first->GetMAC());
			arp_layer->SetTargetIP(arp_layer->GetSenderIP());
			arp_layer->SetSenderIP(pos->first);
			eth_layer->SetSourceMAC(pos->second.first->GetMAC());
			send_connection = (const_cast<CConnection*>(con_pos->second.first));

			Packet * newpkt= new Packet();
			newpkt->PacketFromEthernet(pkt->GetRawPtr(),42);
			send_connection->SendPacket(newpkt,GetRouterNumber());
			delete newpkt;
		}
	}
}
void CRouter::HandleIPv4(Packet * pkt,const double popTime)
{
	CConnection* send_connection;
	map<string,pair< CConnection const*,string> >::iterator pos;
	IP* ip_layer = pkt->GetLayer<IP>();
	pos = mRoutingTable.find(ip_layer->GetDestinationIP());
	if (pos != mRoutingTable.end())
	{
		send_connection = (const_cast<CConnection*>(pos->second.first));
		string dest_ip = send_connection->getGetwayAddress()->getIpStr();
		if (!dest_ip.compare(pos->first))
		{SReport::getInstance().LogPacket(ip_layer->GetIdentification(),ip_layer->GetTotalLength(),mRouterNumber,popTime,mFillage,mDropRate,true);}
		else
		{
			Ethernet* eth_layer = pkt->GetLayer<Ethernet>();
			if (eth_layer != NULL
					&& eth_layer->GetSourceMAC().compare(
							send_connection->GetMAC()))
			{
				if (ProcessSendPacket (pkt))
				{
					send_connection->SendPacket(pkt,GetRouterNumber());
					if(send_connection->isPhysical())
					{
						SReport::getInstance().LogPacket(ip_layer->GetIdentification(),ip_layer->GetTotalLength(),mRouterNumber,popTime,mFillage,mDropRate,true);
					}
					else
					{
						SReport::getInstance().LogPacket(ip_layer->GetIdentification(),ip_layer->GetTotalLength(),mRouterNumber,popTime,mFillage,mDropRate,false);
					}
				}
			}
		}
	}
	else
	{
		SReport::getInstance().LogPacket(ip_layer->GetIdentification(),ip_layer->GetTotalLength(),mRouterNumber,popTime,mFillage,mDropRate,true);
	}
}



void CRouter::Sniff()
{
	Packet* temp_packet;
	boost::random::uniform_real_distribution<> dropRate(0,100);
	boost::random::mt19937 rng;
	double randomizedDropRate;
	try
	{
		while(true)
		{
			if (!mDropRateArr.empty())
			{
				DynamicDropRateHandler();
			}
			list< CConnection const *>::iterator iter = mConnections.begin();
			for (;iter!=mConnections.end();iter++)
			{
				CConnection * connection = const_cast<CConnection*>(*iter);
				boost::this_thread::interruption_point();
				temp_packet= connection->GetPacket(GetRouterNumber());
				if (temp_packet != NULL)
				{
					randomizedDropRate= dropRate(rng);
					boost::this_thread::interruption_point();
					if (randomizedDropRate>=GetDropRate())
					{
						mPacketCollector->PushBack(temp_packet);
					}
					else
					{
						SDataController::getInstance().incData(SDataController::PACKETDROP);
					}
				}
			}
			if (!mThreaded) {break;} //loop once when not threaded
		}
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}
void CRouter::StopEmulation()
{
	if (mThreaded)
	{
		mSniffingThread.interrupt();
		mPacketsHandlingThread.interrupt();
	}
}
CRouter::~CRouter()
{
	try
	{
		mConnections.clear();
		if(mPacketCollector!=NULL)
		{
			delete mPacketCollector;
			mPacketCollector=NULL;
		}
	}
	catch(CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

void CRouter::SetDynamicFillageArray(string str)
{
	try
	{
		std::istringstream in(str);
		double percentage;
		double intervalTime;
		//If no time given the last percentage will not be inserted
		while (in >> percentage && in >> intervalTime)
		{
			if (intervalTime > MAX_INTERVAL_BETWEEN_FILLAGE_CHANGE
					|| intervalTime < 0)
			{
				throw(CException("Interval time is too long"));
			}
			if (percentage >= MAX_FILLAGE_RATE || percentage < 0)
			{
				throw(CException("Too high fillage percentage"));
			}
			mFillageArr.push_back(
					pair<unsigned int, unsigned int>(percentage, intervalTime));
		}
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}
void CRouter::SetDynamicDropRateArray(string str)
{
	try
	{
		std::istringstream in(str);
		double dropRate;
		double intervalTime;
		//If no time given the last percentage will not be inserted
		while (in >> dropRate && in >> intervalTime)
		{
			if (intervalTime > MAX_INTERVAL_BETWEEN_FILLAGE_CHANGE
					|| intervalTime < 0)
			{
				throw(CException("Interval time is too long"));
			}
			if (dropRate >= MAX_DROP_RATE || dropRate < 0)
			{
				throw(CException("Too high drop rate percentage"));
			}
			mDropRateArr.push_back(
					pair<unsigned int, unsigned int>(dropRate, intervalTime));
		}
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}
