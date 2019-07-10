#include "stdafx.h"
#include "autoPort.h"
#include "crt310/CRT_310.h"

AutoPort::AutoPort(std::string p, int baud)
{	

	Xfs::getInstance()->l.slog("Opening port");
	   
	int rate = 4;
	if (baud == 1200)rate = 1;
	if (baud == 2400)rate = 2;
	if (baud == 4800)rate = 3;
	if (baud == 9600)rate = 4;
	if (baud == 19200)rate = 5;
	if (baud == 38400)rate = 6;
	if (baud == 57600)rate = 7;
	port = CommOpenWithBaut((char *)p.c_str(), 4);


	if (!port)
	{
		GetErrCode(&ec);
		Xfs::getInstance()->l.slog("Something went wrong...ec=" + std::to_string(ec));

	}
	else
	{
		Xfs::getInstance()->l.slog("Port opened");
	}

}




AutoPort::~AutoPort()
{
	if(port)
	{ 
		CommClose(port);
	}
}
