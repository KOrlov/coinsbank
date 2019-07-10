#pragma once
class AutoPort
{
public:
	HANDLE port = 0;
	int  ec = 0;

	AutoPort(std::string port, int baud);	
	~AutoPort();
};

