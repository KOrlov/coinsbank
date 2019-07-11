#pragma once
#include <string>
using namespace std;


class Logger
{
private:
	DWORD m_traceLevel;
	CRITICAL_SECTION m_cs;
	string name = "c:\\logs\\default.log";
public:
	Logger();
	~Logger();
	
	//Simple log, just for debug 
	void debug(std::string header,unsigned char *data, int len);
	void debug(std::string header, char *data, int len);
	void debug(const string & s);

	
	   

	void setName(const string & s);
	void setTraceLevel(DWORD level);



	void log(std::string header, unsigned char *data, int len, int level);
	void log(std::string header, char *data, int len, int level);
	void log(const string & s, int level);

	
};

