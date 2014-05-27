
#ifndef SLOGGER_H_
#define SLOGGER_H_

#include <fstream>
#include <stdarg.h>
#include <boost/timer.hpp>
#include <boost/signals2/mutex.hpp>
#include <crafter.h>
#include <map>
#include <set>
#include <sstream>
using std::ofstream;

class SLogger
{
public:
	static SLogger& getInstance()
	{
		static SLogger instance;
		return instance;
	}
	void InitLogger()
	{
		time_t t = time(0);   // get time now
		std::stringstream fileNameAddons;
		struct tm * now = localtime(&t);
		fileNameAddons << "Logger-" << (now->tm_year + 1900) << '-'
				<< (now->tm_mon + 1) << '-' << now->tm_mday<<'-'<<now->tm_hour<<'-'<<now->tm_min<<'-'<< now->tm_sec<<".txt";
		fd.open(fileNameAddons.str().c_str(), std::fstream::out | std::fstream::trunc);
		if (!fd.is_open())
		{
			std::cout << "Can't open log file for write.\n";
			exit (EXIT_FAILURE);
		}
		timer.restart();
	}
	void DestroyLogger()
	{
		fd.close();
	}
	void Log(const char * toLog)
	{
		LoggerMTX.lock();
		fd << timer.elapsed() << " : " << toLog << std::endl;
		LoggerMTX.unlock();
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
	boost::signals2::mutex LoggerMTX;
	SLogger(){};
	SLogger(SLogger const &){}
	void operator=(SLogger const &);
	//logger for analysis - < <packet id> - set<time> >
	std::map<long long int,std::set< std::pair<double,unsigned int> > > PacketLogger;

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
#endif /* SLOGGER_H_ */
