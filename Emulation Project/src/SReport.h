
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

struct classcomp {
  bool operator() (const std::pair< std::pair<double,double> ,unsigned int >& lhs, const std::pair< std::pair<double,double> ,unsigned int >& rhs) const
  {
	  if (lhs.first.first<rhs.first.first)
	  {
		  return true;
	  }else
	  {
		  if(lhs.first.first==rhs.first.first && lhs.first.second<rhs.first.second)
		  {
			  return true;
		  }else
		  {
			  if (lhs.first.first==rhs.first.first && lhs.first.second==rhs.first.second && lhs.second!=rhs.second)
			  {
				  return true;
			  }
		  }
	  }
	  return false;
  }
};
#define TIME_OF_FLUSH_TO_DISK_IN_SECONDS 5
#define TIME_OF_ADD_REPORT 3
#define FILENAME "EData/Report-"
typedef std::pair<double,double> InsertExit;
typedef std::pair< InsertExit ,unsigned int > InsertExitRouter;
typedef std::vector< InsertExitRouter> InsertExitRouterSet;
typedef std::pair<long long int,long long int> PacketIDSize;
typedef std::map< PacketIDSize,InsertExitRouterSet > PacketInfoMap;


#define KB_SIZE_IN_BYTES 1024
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
		PDcount = 0;
		SDcount = 0;
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
		ss << "var PDcount = " << PDcount << ";";
		ss << "var SDcount = " << SDcount << ";";
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
			PacketReport[newInsertKey].push_back(InsertExitRouter(InsertExit(insertTime,timeD.total_milliseconds()),routerNumber));
			graphAverageFillage+=mFillage;
			graphAverageDropRate+=mDropRate;
			totalPacketsTransferred++;
			if (hasExitedEmulation)
			{
				double totalTimeUntilExit=0;

				InsertExitRouterSet::iterator it;
				for (it = PacketReport[newInsertKey].begin();it!= PacketReport[newInsertKey].end(); it++)
				{
					ss << "{'type':'PD','pID':"<< packetID<< ",'pSize':"<< packetSize;
					graphSpeedCalcSize+=newInsertKey.second; // add the packet size
					totalTimeUntilExit+=((*it).first.second - (*it).first.first);

					ss<< ",'rID':" << (*it).second<< ",'insT':" <<
							(*it).first.first << ",'exitT':"  <<
							(*it).first.second<< ",'totalT':" <<
							((*it).first.second - (*it).first.first)<<
							",'fil':" << mFillage<< ",'dp':" <<
							mDropRate<<",'ue':" << totalTimeUntilExit<<
							"},"<<std::endl;
					PDcount++;
				}
				PacketReport.erase(newInsertKey);
			}
		}
		if (timeInSec - lastFlushTime > TIME_OF_ADD_REPORT)
		{
			graphAverageFillage = (totalPacketsTransferred!=0)?(graphAverageFillage / totalPacketsTransferred):0;
			graphAverageDropRate = (totalPacketsTransferred!=0)?graphAverageDropRate
					/ totalPacketsTransferred : 0;
			double avgSpeed = (graphSpeedCalcSize
					/(KB_SIZE_IN_BYTES *TIME_OF_ADD_REPORT));

			ss << "{'type':'SD','gaf':" << graphAverageFillage << ",'gadr':"
					<< graphAverageDropRate << ",'asp':" << avgSpeed << ",'te':"
					<< timeInSec << "}," << std::endl;
			SDcount++;
			graphSpeedCalcSize = 0;
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

	double GetReportElapsedTimeInSeconds()
	{
		ReportMTX.lock();
		boost::posix_time::time_duration timeD=boost::posix_time::microsec_clock::local_time()-startTime;
		ReportMTX.unlock();
		return timeD.total_seconds();
	}

	boost::posix_time::time_duration GetReportElapsedTimeInTD()
	{
		ReportMTX.lock();
		boost::posix_time::time_duration timeD=boost::posix_time::microsec_clock::local_time()-startTime;
		ReportMTX.unlock();
		return timeD;
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
	SReport():lastFlushTime(0),graphSpeedCalcSize(0),totalPacketsTransferred(0),graphAverageFillage(0),graphAverageDropRate(0),PDcount(0),SDcount(0){};
	SReport(SReport const &):lastFlushTime(0),graphSpeedCalcSize(0),totalPacketsTransferred(0),graphAverageFillage(0),graphAverageDropRate(0),PDcount(0),SDcount(0){}
	void operator=(SReport const &);
	//Report for analysis - < <packet id> - set<time> >
	PacketInfoMap PacketReport;
	double graphSpeedCalcSize;
	unsigned int totalPacketsTransferred;
	double graphAverageFillage;
	double graphAverageDropRate;
	long long int PDcount;
	long long int SDcount;
};
#endif /* SREPORT_H_ */
