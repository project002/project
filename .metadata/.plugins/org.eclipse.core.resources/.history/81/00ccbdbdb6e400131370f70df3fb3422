
#ifndef SREPORT_H_
#define SREPORT_H_

#include <fstream>
#include <stdarg.h>
#include <boost/timer.hpp>
#include <boost/signals2/mutex.hpp>
#include <crafter.h>
#include <map>
#include <set>
using std::ofstream;

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
		fd.open("Report.html", std::fstream::out | std::fstream::trunc);
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
	void LogPacket(long long int packetID,unsigned int routerNumber,double insertTime, bool hasExitedEmulation=false)
	{
		ReportMTX.lock();
		if (packetID!=0)
		{
			double timeElapsed=timer.elapsed();
			PacketReport[packetID].insert(std::pair<double,unsigned int>(timeElapsed,routerNumber));
			if (hasExitedEmulation)
			{
				fd << "<tr><td>" << "Packet ID: "<< "</td><td>"<< packetID<< "</td><td>";
				std::set< std::pair<double,unsigned int> >::iterator it;
				for (it = PacketReport[packetID].begin();it!= PacketReport[packetID].end(); it++)
				{
					fd<< "</td><td>" << "Router Number: "<< "</td><td>" << (*it).second<< "</td><td>" << " Insert Time: "<< "</td><td>" << insertTime << "</td><td>" <<" Exit Time "<< "</td><td>" << (*it).first<< "</td></tr>";
				}
				fd<<std::endl;
				PacketReport.erase(packetID);
			}
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
	ofstream fd;
	boost::timer timer;
	boost::signals2::mutex ReportMTX;
	SReport(){};
	SReport(SReport const &){}
	void operator=(SReport const &);
	//Report for analysis - < <packet id> - set<time> >
	std::map<long long int,std::set< std::pair<double,unsigned int> > > PacketReport;

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
