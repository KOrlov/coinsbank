#include "stdafx.h"
#include "Logger.h"

#include "AutoLock.h"
#include "time.h"
#include "xfs.h"
#include "boost/date_time/posix_time/posix_time.hpp" 
#include "boost/algorithm/hex.hpp"

#pragma warning(disable : 4996) 
Logger::Logger()
{
	InitializeCriticalSection(&m_cs);
}


Logger::~Logger()
{
	DeleteCriticalSection (&m_cs);
}

void Logger::debug(std::string header,unsigned char *data, int len)
{
	log(header, data, len, 4);
}
void Logger::debug(std::string header,  char *data, int len)
{
	log(header, data, len, 4);
}
void Logger::debug(const string & s)
{	
	log(s,4);
}






void Logger::log(std::string header, unsigned char *data, int len, int level = 0)
{
	std::vector<unsigned char> v;
	for (int i = 0; i < len; i++)
	{
		v.push_back(data[i]);
	}
	std::string res;
	boost::algorithm::hex(v.begin(), v.end(), back_inserter(res));

	std::string pr = header + "(" + std::to_string(len) + "):" + res;
	log(pr,level);
}
void Logger::log(std::string header, char *data, int len, int level = 0)
{
	std::vector<char> v;
	for (int i = 0; i < len; i++)
	{
		v.push_back(data[i]);
	}
	std::string res;
	boost::algorithm::hex(v.begin(), v.end(), back_inserter(res));

	std::string pr = header + "(" + std::to_string(len) + "):" + res;
	log(pr,level);
}
void Logger::log(const string & s, int level=0)
{
	//LOCK COMMENTED
	//
	AutoLock lock(&m_cs);

	try {
		//MUTEX ADDED
		//if (WaitForSingleObject(Xfs::getInstance()->hMutex, INFINITE) != WAIT_OBJECT_0)return;
		time_t t = time(NULL);
		struct tm* curtime = localtime(&t);

		ofstream file;

		char buf[80];
		strftime(buf, sizeof(buf), "_%d%m%y.log", curtime);
		string ft(buf);
		string newname(name + ft);
		file.open(newname, fstream::app);


		std::string sTime = boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time());

		//file << "\n" << asctime(curtime) << ":" << s << "\n";
		file << "\n" << sTime.substr(9) + " :" + s << "\n";
		file.close();
	}
	catch (...)
	{
	}
	//MUTEX ADDED
	//ReleaseMutex(Xfs::getInstance()->hMutex);

}




void Logger::setName(const string & s)
{
	name = s;
}

void Logger::setTraceLevel(DWORD level)
{
	m_traceLevel = level;
}

