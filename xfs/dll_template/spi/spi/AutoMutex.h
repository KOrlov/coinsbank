#pragma once
class AutoMutex
{
	HANDLE m_mutex = 0;


public:
	AutoMutex(HANDLE m);
	~AutoMutex();
};

