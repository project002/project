
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


#define TIME_OF_FLUSH_TO_DISK_IN_SECONDS 5
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
//
//		gd.open((std::string("Graphs-")+reportDateAndTimeString.str()).c_str(), std::fstream::out | std::fstream::trunc);
//		if (!gd.is_open())
//		{
//			std::cout << "Can't open graph file for write.\n";
//			exit (EXIT_FAILURE);
//		}
		std::stringstream ss;
		ss << "<!DOCTYPE html><html><head><title>Emulation Report</title>" ;
		ss << "<meta charset='utf-8'>";
		ss << "<link rel='script/stylesheet' type='text/css' href=href='report.css'/>";
		ss << "<script src='script/jquery-2.1.1.min.js'></script>";
		ss << "<script src='script/Chart.min.js'></script>";
		ss << "<script src='script/report.js'></script>";
		ss << "</head><body><div id='mc'><table>";

		timer.restart();
		rawLog(fd,ss.str().c_str());
		//TODO: verify GD initialization is correct
//		rawLog(gd,"<!DOCTYPE html><html><head><title>Emulation Report</title></head><body><canvas id=\"myChart\" width=\"400\" height=\"400\"></canvas>");
	}
	void DestroyReport()
	{
		rawLog(fd,"</table></div></body></html>");

		//TODO: verify GD finishialization is correct
//		rawLog(gd,"<script src=\"Chart.js\"></script></body></html>");
		fd.close();
//		gd.close();
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
		fd << "<tr><td>" << timer.elapsed() << "</td><td>" << toLog << "</td></tr>";
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
		double timeElapsed=timer.elapsed();
		std::pair<long long int, long long int> newInsertKey = std::pair<long long int, long long int>(packetID,packetSize);
		if (packetID!=0)
		{
			PacketReport[newInsertKey].insert(std::pair<double,unsigned int>(timeElapsed,routerNumber));
			graphAverageFillage+=mFillage;
			graphAverageDropRate+=mDropRate;
			totalPacketsTransferred++;
			if (hasExitedEmulation)
			{
				double totalTimeUntilExit=0;
				ss << "<tr class='packetData'><td>" << "Packet ID: "<< "</td><td class='pID'>"<< packetID<< "</td><td>" << "Packet Size: "<< "</td><td class='pSize'>"<< packetSize<< "</td><td>";
				std::set< std::pair<double,unsigned int> >::iterator it;
				for (it = PacketReport[newInsertKey].begin();it!= PacketReport[newInsertKey].end(); it++)
				{
					graphSpeedCalcSize+=newInsertKey.second; // add the packet size
					totalTimeUntilExit+=((*it).first - insertTime);
					graphSpeedCalcTimer+=totalTimeUntilExit; // add packet transfer time

					ss<< "</td><td>" << "Router Number: "<< "</td><td class='rID'>" << (*it).second<< "</td><td>" << " Insert Time: "<< "</td><td class='insT'>" << insertTime << "</td><td>" <<" Exit Time "<< "</td><td class='exitT'>" << (*it).first<< "</td><td>" <<" Total Time In Router "<< "</td><td class='totalT'>" << (*it).first - insertTime<< "</td><td>" <<" Fillage "<< "</td><td class='fillage'>" << mFillage<< "</td><td>" <<" DropRate "<< "</td><td class='droprate'>" << mDropRate<< "</td>";
				}
				ss<<"<td>" <<" Total Time "<< "</td><td>" << totalTimeUntilExit<< "</td></tr>"<<std::endl;
				PacketReport.erase(newInsertKey);
			}
		}
		if (timeElapsed - lastFlushTime > TIME_OF_FLUSH_TO_DISK_IN_SECONDS)
		{
			fd.write(ss.str().c_str(),ss.str().size());
			ss.clear();
			graphAverageFillage=graphAverageFillage/totalPacketsTransferred;
			graphAverageDropRate=graphAverageDropRate/totalPacketsTransferred;

			//TODO: add to 'gd' file the graph properties
			// 'timer' - X axis , time that passed since emulation started
			// 'graphAverageFillage' - Y axis average fillage
			// 'graphAverageDropRate' - Y axis average fillage
			// (('1000' / 'graphSpeedCalcTimer') * 'graphSpeedCalcSize') - Y axis - Kbytes per second
			// total of 4 lines graph.

			graphSpeedCalcSize=0;
			graphSpeedCalcTimer=0;
			totalPacketsTransferred=0;
			graphAverageFillage=0;
			graphAverageDropRate=0;
			lastFlushTime=timeElapsed;
		}
		ReportMTX.unlock();
	}
	void LogRouter(unsigned int RouterNumber, unsigned int BufferSize, double DropRate, unsigned int BufferUsedSize, double Fillage)
	{
		ReportMTX.lock();
		fd<< "<tr class='routerData'><td colspan='2'>" <<"Router: "<< "</td><td class='rID'>" <<RouterNumber<< "</td><td colspan='2' >" <<" Buffer Size: " <<"</td><td class='buffersize'>" <<BufferSize << "</td><td colspan='2' >" <<" DropRate: " <<"</td><td><span class='droprate'>" << DropRate << "</span>" << "%" << "</td><td colspan='2'>" << "Buffer Initial Usage: " << "</td><td>" << BufferUsedSize<< "</td><td colspan='2'>" << " Fillage: " <<"</td><td><span class='fillage'>" <<Fillage<< "</span>" << "%" << "</td></tr>" ;
		fd<<std::endl;
		ReportMTX.unlock();
	}
	double GetReportElapsedTime()
	{
		ReportMTX.lock();
		double timeElapsed=timer.elapsed();
		ReportMTX.unlock();
		return timeElapsed;
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
	double lastFlushTime;
	//file descriptor
	ofstream fd;
	//graphs descriptor
	//ofstream gd;
	std::stringstream ss;
	boost::timer timer;
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
