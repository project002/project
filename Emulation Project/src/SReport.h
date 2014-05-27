
#ifndef SREPORT_H_
#define SREPORT_H_

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
		std::stringstream fileNameAddons;
		struct tm * now = localtime(&t);
		fileNameAddons <<"Report-"<< (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-'
				<< now->tm_mday<<'-'<<now->tm_hour<<'-'<<now->tm_min<<'-'<< now->tm_sec<<".html";
		fd.open(fileNameAddons.str().c_str(), std::fstream::out | std::fstream::trunc);
		if (!fd.is_open())
		{
			std::cout << "Can't open log file for write.\n";
			exit (EXIT_FAILURE);
		}
		timer.restart();
		rawLog("<!DOCTYPE html><html><head><title>Emulation Report</title></head><body><table>");
	}
	void DestroyReport()
	{
		rawLog("</table></body></html>");
		fd.close();
	}

	void rawLog(const char * toLog)
	{
		ReportMTX.lock();
		fd << toLog << std::endl;
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
			if (hasExitedEmulation)
			{
				double totalTimeUntilExit=0;
				ss << "<tr><td>" << "Packet ID: "<< "</td><td>"<< packetID<< "</td><td>" << "Packet Size: "<< "</td><td>"<< packetSize<< "</td><td>";
				std::set< std::pair<double,unsigned int> >::iterator it;
				for (it = PacketReport[newInsertKey].begin();it!= PacketReport[newInsertKey].end(); it++)
				{
					totalTimeUntilExit+=((*it).first - insertTime);
					ss<< "</td><td>" << "Router Number: "<< "</td><td>" << (*it).second<< "</td><td>" << " Insert Time: "<< "</td><td>" << insertTime << "</td><td>" <<" Exit Time "<< "</td><td>" << (*it).first<< "</td><td>" <<" Total Time In Router "<< "</td><td>" << (*it).first - insertTime<< "</td><td>" <<" Fillage "<< "</td><td>" << mFillage<< "</td><td>" <<" DropRate "<< "</td><td>" << mDropRate<< "</td>";
				}
				ss<<"<td>" <<" Total Time "<< "</td><td>" << totalTimeUntilExit<< "</td></tr>"<<std::endl;
				PacketReport.erase(newInsertKey);
			}
		}
		if (timeElapsed - lastFlushTime > TIME_OF_FLUSH_TO_DISK_IN_SECONDS)
		{
			fd.write(ss.str().c_str(),ss.str().size());
			ss.clear();
			lastFlushTime=timeElapsed;
		}
		ReportMTX.unlock();
	}
	void LogRouter(unsigned int RouterNumber, unsigned int BufferSize, double DropRate, unsigned int BufferUsedSize, double Fillage)
	{
		ReportMTX.lock();
		fd<< "<tr><td colspan='2'>" <<"Router: "<< "</td><td>" <<RouterNumber<< "</td><td colspan='2' >" <<" Buffer Size: " <<"</td><td>" <<BufferSize << "</td><td colspan='2' >" <<" DropRate: " <<"</td><td>" << DropRate << "%" << "</td><td colspan='2'>" << "Buffer Initial Usage: " << "</td><td>" << BufferUsedSize<< "</td><td colspan='2'>" << " Fillage: " <<"</td><td>" <<Fillage<< "%" << "</td></tr>" ;
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
	ofstream fd;
	std::stringstream ss;
	boost::timer timer;
	boost::signals2::mutex ReportMTX;
	SReport():lastFlushTime(0){};
	SReport(SReport const &):lastFlushTime(0){}
	void operator=(SReport const &);
	//Report for analysis - < <packet id> - set<time> >
	std::map< std::pair<long long int,long long int>,std::set< std::pair<double,unsigned int> > > PacketReport;

};






/*
#define COLOR_NORMAL "\x1B[0m"
#define COLOR_RED "\x1B[31m"
#define COLOR_GREEN "\x1B[32m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_BLUE "\x1B[34m"
#define COLOR_WHITE "\x1B[37m"

static inline void LogColor(const char * text, const char * color = COLOR_NORMAL)
{
	printf("%s%s",color,text);
	printf("%s",COLOR_NORMAL);
}

static inline void LogColorChange(const char * color = COLOR_NORMAL)
{
	printf("%s",color);
}
static inline void LogColorReset()
{
	printf("%s",COLOR_NORMAL);
}*/
#endif /* SREPORT_H_ */
