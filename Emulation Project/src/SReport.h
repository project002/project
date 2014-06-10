
#ifndef SREPORT_H_
#define SREPORT_H_

#include <iostream>
#include <fstream>
#include <stdarg.h>
#include <boost/timer.hpp>
#include <boost/signals2/mutex.hpp>
#include <crafter.h>
#include <map>
#include <set>
#include <sstream>
#include <ctime>
using std::ofstream;
#include "boost/date_time/posix_time/posix_time.hpp"

#define TIME_OF_FLUSH_TO_DISK_IN_SECONDS 5
#define TIME_OF_ADD_REPORT 3
#define FILENAME "EData/Report-"

class SReport
{
public:
	static SReport& getInstance()
	{
		static SReport instance;
		return instance;
	}
	void InitReport()
	{
		lastFlushTime=0;
		time_t t = time(0);   // get time now
		std::stringstream reportDateAndTimeString;

		struct tm * now = localtime(&t);
		reportDateAndTimeString<< (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-'
						<< now->tm_mday<<'-'<<now->tm_hour<<'-'<<now->tm_min<<'-'<< now->tm_sec<<".html";

		fd.open((std::string(FILENAME)+reportDateAndTimeString.str()).c_str(), std::fstream::out | std::fstream::trunc);
		if (!fd.is_open())
		{
			std::cout << "Can't open report file for write.\n";
			exit (EXIT_FAILURE);
		}

		std::stringstream ss;
		ss << "<!DOCTYPE html><html><head><title>Emulation Report</title>" ;
		ss << "<meta charset='utf-8'>";
		ss << "<link rel='stylesheet' type='text/css' href='script/report.css'/>";
		ss << "<script> var ts = [";
		startTime = boost::posix_time::microsec_clock::local_time();

		rawLog(fd,ss.str().c_str());
	}
	void DestroyReport()
	{
		std::stringstream ss;
		ss << "{}];";
		ss << "</script>";
		ss << "<script src='script/jquery-2.1.1.min.js'></script>";
		ss << "<script src='script/Chart.min.js'></script>";
		ss << "<script src='script/report.js'></script>";
		ss << "</head><body></body></html>";
		rawLog(fd,ss.str().c_str());

		fd.close();
	}

	void rawLog(ofstream & desc , const char * toLog)
	{
		ReportMTX.lock();
		desc << toLog << std::endl;
		ReportMTX.unlock();
	}

	void Log(const char * toLog)
	{
		ReportMTX.lock();
		boost::posix_time::time_duration timeD=boost::posix_time::microsec_clock::local_time()-startTime;
		fd << "<tr><td>" << timeD.total_milliseconds() << "</td><td>" << toLog << "</td></tr>";
		ReportMTX.unlock();
	}

	/**
	 *
	 * @param packetID
	 * @param routerNumber
	 * @param hasExitedEmulation
	 */
	void LogPacket(long long int packetID,long long int packetSize,unsigned int routerNumber,double insertTime,double mFillage,double mDropRate, bool hasExitedEmulation=false)
	{
		ReportMTX.lock();
		boost::posix_time::time_duration timeD=boost::posix_time::microsec_clock::local_time()-startTime;
		double timeInSec =timeD.total_seconds();
		std::pair<long long int, long long int> newInsertKey = std::pair<long long int, long long int>(packetID,packetSize);
		if (packetID!=0)
		{
			PacketReport[newInsertKey].insert(std::pair<double,unsigned int>(insertTime,routerNumber));
			graphAverageFillage+=mFillage;
			graphAverageDropRate+=mDropRate;
			totalPacketsTransferred++;
			if (hasExitedEmulation)
			{
				double totalTimeUntilExit=0;
				//ss << "<tr class='packetData'><td>" << "Packet ID: "<< "</td><td class='pID'>"<< packetID<< "</td><td>" << "Packet Size: "<< "</td><td class='pSize'>"<< packetSize<< "</td><td>";
				ss << "{'type':'PD','pID':"<< packetID<< ",'pSize':"<< packetSize;
				std::set< std::pair<double,unsigned int> >::iterator it;
				for (it = PacketReport[newInsertKey].begin();it!= PacketReport[newInsertKey].end(); it++)
				{
					graphSpeedCalcSize+=newInsertKey.second; // add the packet size
					totalTimeUntilExit+=((*it).first - insertTime);
					graphSpeedCalcTimer+=totalTimeUntilExit; // add packet transfer time

					//ss<< "</td><td>" << "Router Number: "<< "</td><td class='rID'>" << (*it).second<< "</td><td>" << " Insert Time: "<< "</td><td class='insT'>" << insertTime << "</td><td>" <<" Exit Time "<< "</td><td class='exitT'>" << (*it).first<< "</td><td>" <<" Total Time In Router "<< "</td><td class='totalT'>" << (*it).first - insertTime<< "</td><td>" <<" Fillage "<< "</td><td class='fillage'>" << mFillage<< "</td><td>" <<" DropRate "<< "</td><td class='droprate'>" << mDropRate<< "</td>";
					ss<< ",'rID':" << (*it).second<< ",'insT':" << insertTime << ",'exitT':" << (*it).first<< ",'totalT':" << (*it).first - insertTime<< ",'fil':" << mFillage<< ",'dp':" << mDropRate;
				}
				//ss<<"<td>" <<" Total Time "<< "</td><td class='untilExit'>" << totalTimeUntilExit<< "</td></tr>"<<std::endl;
				ss<<",'ue':" << totalTimeUntilExit<< "},"<<std::endl;
				PacketReport.erase(newInsertKey);
			}
		}
		if (timeInSec - lastFlushTime > TIME_OF_ADD_REPORT)
		{
			graphAverageFillage = (totalPacketsTransferred!=0)?(graphAverageFillage / totalPacketsTransferred):0;
			graphAverageDropRate = (totalPacketsTransferred!=0)?graphAverageDropRate
					/ totalPacketsTransferred : 0;
			double avgSpeed = (graphSpeedCalcSize
					/ (1000 / TIME_OF_ADD_REPORT));

			ss << "{'type':'SD','gaf':" << graphAverageFillage << ",'gadr':"
					<< graphAverageDropRate << ",'asp':" << avgSpeed << ",'te':"
					<< timeInSec << "}," << std::endl;

			graphSpeedCalcSize = 0;
			graphSpeedCalcTimer = 0;
			totalPacketsTransferred = 0;
			graphAverageFillage = 0;
			graphAverageDropRate = 0;

			fd.write(ss.str().c_str(), ss.str().size());
			ss.str(std::string());
			lastFlushTime = timeInSec;

		}

		ReportMTX.unlock();
	}
	void LogRouter(unsigned int RouterNumber, unsigned int BufferSize, double DropRate, unsigned int BufferUsedSize, double Fillage)
	{
		ReportMTX.lock();
		//fd<< "<tr class='routerData'><td colspan='2'>" <<"Router: "<< "</td><td class='rID'>" <<RouterNumber<< "</td><td colspan='2' >" <<" Buffer Size: " <<"</td><td class='buffersize'>" <<BufferSize << "</td><td colspan='2' >" <<" DropRate: " <<"</td><td><span class='droprate'>" << DropRate << "</span>" << "%" << "</td><td colspan='2'>" << "Buffer Initial Usage: " << "</td><td>" << BufferUsedSize<< "</td><td colspan='2'>" << " Fillage: " <<"</td><td><span class='fillage'>" <<Fillage<< "</span>" << "%" << "</td></tr>" ;
		fd<< "{'type':'RD','rID':" <<RouterNumber<< ",'bsz':" <<BufferSize << ",'dp':" << DropRate << ",'bus':" << BufferUsedSize<< ",'fil':" <<Fillage<< "}," ;
		fd<<std::endl;
		ReportMTX.unlock();
	}
	double GetReportElapsedTime()
	{
		ReportMTX.lock();
		boost::posix_time::time_duration timeD=boost::posix_time::microsec_clock::local_time()-startTime;
		ReportMTX.unlock();
		return timeD.total_milliseconds();
	}
	void Logf(const char* format,...)
	{
		va_list args;
		va_start(args,format);
		char str[256] = {0};
		vsprintf(str,format,args);
		va_end(args);
		Log(str);

	}


private:
	boost::posix_time::ptime startTime;
	double lastFlushTime;
	//file descriptor
	ofstream fd;
	std::stringstream ss;
	boost::signals2::mutex ReportMTX;
	SReport():lastFlushTime(0),graphSpeedCalcSize(0),graphSpeedCalcTimer(0),totalPacketsTransferred(0),graphAverageFillage(0),graphAverageDropRate(0){};
	SReport(SReport const &):lastFlushTime(0),graphSpeedCalcSize(0),graphSpeedCalcTimer(0),totalPacketsTransferred(0),graphAverageFillage(0),graphAverageDropRate(0){}
	void operator=(SReport const &);
	//Report for analysis - < <packet id> - set<time> >
	std::map< std::pair<long long int,long long int>,std::set< std::pair<double,unsigned int> > > PacketReport;
	double graphSpeedCalcSize;
	double graphSpeedCalcTimer;
	unsigned int totalPacketsTransferred;
	double graphAverageFillage;
	double graphAverageDropRate;
};

#endif /* SREPORT_H_ */
