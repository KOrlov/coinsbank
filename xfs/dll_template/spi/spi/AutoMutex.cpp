#include "stdafx.h"
#include "AutoMutex.h"


AutoMutex::AutoMutex(HANDLE m)
{
	m_mutex = m;

	if (WaitForSingleObject(m_mutex, INFINITE) != WAIT_OBJECT_0)return;
}


AutoMutex::~AutoMutex()
{
	ReleaseMutex(m_mutex);
}
