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
	void slog(std::string header,unsigned char *data, int len);
	void slog(std::string header, char *data, int len);

	void slog(const string & s);

	void setName(const string & s);
	void setTraceLevel(DWORD level);
	void trace(const string &data, DWORD level);
		
	
};

