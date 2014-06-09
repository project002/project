/*
 * CPacketCollector.cpp
 *
 *  Created on: Dec 17, 2013
 *      Author: root
 */

#include "CPacketCollector.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
CPacketCollector::CPacketCollector(unsigned int bufferSize):mBufferSize(bufferSize)
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

CPacketCollector::~CPacketCollector()
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

/**
 * Adding number of fake packets to packet collector
 * FIXME: TODO:if the program crash it probably means that delete packet isnt deleting the layers allocated in this function
 * or in case the layers arent dynamic it means that they should be otherwise they are deleted here and gone forever.
 * @param numberOfPackets
 */
void CPacketCollector::AddRandomPackets(unsigned int numberOfPackets,double DropRate)
{
	try
	{
		boost::random::uniform_real_distribution<> dropRate(0,100);
		boost::random::mt19937 rng;
		double randomizedDropRate;
		mMtx.lock();
		for (unsigned int i = 0; i < numberOfPackets; i++)
		{

			Crafter::Packet * fakePacket=new Packet();

			/* Create an IP header */
			IP ip_header;
			/* Set the Source and Destination IP address */
			ip_header.SetSourceIP("0.0.0.0");
			ip_header.SetDestinationIP("0.0.0.0");

			/* Create an TCP - SYN header */
			TCP tcp_header;
			tcp_header.SetSrcPort(0);
			tcp_header.SetDstPort(0);
			tcp_header.SetSeqNumber(0);
			tcp_header.SetFlags(0);

			RawLayer payload ("ArbitraryPayload");

			fakePacket->PushLayer(ip_header);
			fakePacket->PushLayer(tcp_header);
			fakePacket->PushLayer(payload);
			randomizedDropRate= dropRate(rng);
			if (mPackets.size() < mBufferSize && randomizedDropRate>=DropRate)
			{
				mPackets.push_back(fakePacket);
				mPacketsReceiveTime.push_back(SReport::getInstance().GetReportElapsedTime());
				SDataController::getInstance().incData(
						SDataController::PACKETPROCCES);
			}
			else
			{
				SDataController::getInstance().incData(
						SDataController::PACKETDROP);
				if (fakePacket != NULL)
				{
					delete (fakePacket);
					fakePacket = NULL;
				}
				break;
			}
		}
		mMtx.unlock();
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

void CPacketCollector::PushBack(Crafter::Packet * pkt)
{
	try
	{
		mMtx.lock();
		if (mPackets.size()<mBufferSize)
		{
			mPackets.push_back(pkt);
			mPacketsReceiveTime.push_back(SReport::getInstance().GetReportElapsedTime());
			SDataController::getInstance().incData(SDataController::PACKETPROCCES);
		}
		else
		{
			SDataController::getInstance().incData(SDataController::PACKETDROP);
			if (pkt!=NULL)
			{
				delete (pkt);
				pkt = NULL;
			}
		}
		mMtx.unlock();
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}

Crafter::Packet * CPacketCollector::PopFront(double & popTime)
{
	try
	{
		mMtx.lock();
		if (!mPackets.empty())
		{
			Crafter::Packet * toret = mPackets.front();
			mPackets.pop_front();
			if (mPacketsReceiveTime.empty())
			{
				//no match between packets insterted and timers inserted
				throw (CException("Bad Packets-Timers relation"));
			}
			popTime=mPacketsReceiveTime.front();
			mPacketsReceiveTime.pop_front();
			mMtx.unlock();
			return toret; //syndrom
		}
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
	mMtx.unlock();
	return NULL;

}
/**
 * according to the router properties, its fillage can change
 * The current fillage is calculated and the different in the percentage is calculated
 * as number of packets to be added to the buffer.
 *
 *
 * @param Fillage whats the current wanted fillage.
 */
void CPacketCollector::FixBufferFillage(double Fillage,double DropRate)
{
	try
	{
		mMtx.lock();
		double fillPercentage =((double(mPackets.size())/mBufferSize)*(100.0));
		if (fillPercentage < Fillage)
		{
			mMtx.unlock();
			if (((Fillage-fillPercentage)/100.0)*mBufferSize > 0)
			{
				AddRandomPackets(((Fillage-fillPercentage)/100.0)*mBufferSize,DropRate);
			}
		}
		else
		{
			mMtx.unlock();
		}
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
}
void CPacketCollector::DropRandomPacket()
{
	try
	{
		mMtx.lock();
		if (!mPackets.empty())
		{
			list<Crafter::Packet*>::iterator iter = mPackets.begin();
			unsigned int cellNumber=rand()% mPackets.size();
			for (unsigned int i=0;i<cellNumber-1;i++,iter++);
			if ((*iter)!=NULL)
			{
				delete (*iter);
				(*iter)=NULL;
			}
			mPackets.erase(iter);
		}
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
	mMtx.unlock();
}

/**
 * Function for debugging. It prints the whole packet as it was received.
 * @param buffer pointer to the array's beginning
 * @param recvSize size of the received packet in bytes
 */
void CPacketCollector::PrintPacket()
{
	try
	{
		mMtx.lock();
		mPackets.front()->Print();
		mMtx.unlock();
	}
	catch (CException & error)
	{
		SLogger::getInstance().Log(error.what());
		SLogger::getInstance().Log(__PRETTY_FUNCTION__);
		throw;
	}
	mMtx.unlock();
}

