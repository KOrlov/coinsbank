#include "stdafx.h"
#include "MessageHandler.h"
#include "AutoLock.h"
#include "xfs.h"
#include "XFSADMIN.H"
#include <iostream>
#include <fstream>
#include "time.h"
#include "MemoryManager.h"



MessageHandler::MessageHandler()
{
	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_csmap);
	InitializeCriticalSection(&m_vnrmap);
	InitializeCriticalSection(&m_vnmap);
	InitializeCriticalSection(&m_wcar);
	InitializeCriticalSection(&m_stl);
}

void MessageHandler::start()
{
	{
		AutoLock lock(&m_cs);
		if (started)return;
		bool started = true;
	}

	/*
	Thread<MessageHandler> *t = new Thread<MessageHandler>();
	handlers.push_back(t);
	t->Start(this, &MessageHandler::handlerProc);

		*/

	for (int i = 0; i < 20; i++)
	{
		Thread<MessageHandler> *t = new Thread<MessageHandler>();
		handlers.push_back(t);
		t->Start(this, &MessageHandler::handlerProc);
	}


}

MessageHandler::~MessageHandler()
{
	for (int i = 0; i <20; i++)
	{
		handlers[i]->Stop();
		delete handlers[i];
	}

	DeleteCriticalSection(&m_cs);
	DeleteCriticalSection(&m_csmap);
	DeleteCriticalSection(&m_vnrmap);
	DeleteCriticalSection(&m_vnmap);
	DeleteCriticalSection(&m_wcar);
	DeleteCriticalSection(&m_stl);
}

void MessageHandler::enquee(/*const*/ QueueItem & message)
{
	AutoLock lock(&m_cs);
	message.m_time = time(0);

	m_queue.push_back(message);

}
QueueItem MessageHandler::dequee()
{
	AutoLock lock(&m_cs);
	QueueItem e;
	if (m_queue.empty())return e;

	e = m_queue.front();
	m_queue.pop_front();
	return e;
}


void MessageHandler::handlerProc(HANDLE e)
{
	Xfs::getInstance()->l.slog("Handler");

	while (::WaitForSingleObject(e, 100) == WAIT_TIMEOUT)
	{

		QueueItem m = this->dequee();
		//if (m.length() > 0)
		{
			process(m);
		}
		Sleep(1);
	}
}

void MessageHandler::executeProc(HANDLE e)
{
	Xfs::getInstance()->l.slog("Handler");

	while (::WaitForSingleObject(e, 100) == WAIT_TIMEOUT)
	{

		QueueItem m = this->dequee();
		//if (m.length() > 0)
		{
			process(m);
		}
		Sleep(1);
	}

}
void MessageHandler::timerProc(HANDLE e)
{
	Xfs::getInstance()->l.slog("Handler");

	while (::WaitForSingleObject(e, 100) == WAIT_TIMEOUT)
	{

		QueueItem m = this->dequee();
		//if (m.length() > 0)
		{
			process(m);
		}
		Sleep(1);
	}
}




void MessageHandler::processRegister(const QueueRegisterItem & m)
{


	Xfs::getInstance()->registerEvens(m.m_hService, m.hWndReg, m.dwEventClass);
	Xfs::getInstance()->l.slog("got register");


	LPWFSRESULT res;

	MemoryManager m_memoryManager;
	MemoryAllocator* memAlloc = m_memoryManager.GetAllocator(MEMALLOC_SHM);
	LPWFSRESULT pResult = NULL;

	auto memCheck = memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
	pResult->RequestID = m.m_reqID;
	pResult->hService = m.m_hService;
	pResult->hResult = 0;
	auto ct = ::GetCurrentTime();
	GetSystemTime(&pResult->tsTimestamp);

	pResult->lpBuffer = 0;
	pResult->u.dwCommandCode = 0;

	Xfs::getInstance()->sendMessage(m.m_hWnd, WFS_REGISTER_COMPLETE, (LONG)pResult);

	Xfs::getInstance()->printDebugData();

}

void MessageHandler::processDeregister(const QueueRegisterItem & m)
{

	Xfs::getInstance()->deregisterEvens(m.m_hService, m.hWndReg, m.dwEventClass);
	Xfs::getInstance()->l.slog("got deregister");


	LPWFSRESULT res;

	MemoryManager m_memoryManager;
	MemoryAllocator* memAlloc = m_memoryManager.GetAllocator(MEMALLOC_SHM);
	LPWFSRESULT pResult = NULL;

	auto memCheck = memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
	pResult->RequestID = m.m_reqID;
	pResult->hService = m.m_hService;
	pResult->hResult = 0;
	auto ct = ::GetCurrentTime();
	GetSystemTime(&pResult->tsTimestamp);

	pResult->lpBuffer = 0;
	pResult->u.dwCommandCode = 0;

	Xfs::getInstance()->sendMessage(m.m_hWnd, WFS_DEREGISTER_COMPLETE, (LONG)pResult);

	Xfs::getInstance()->printDebugData();

}


void MessageHandler::processOpen(const QueueOpenItem & m)
{

	Xfs::getInstance()->l.slog("got open");

	LPWFSRESULT res;

	MemoryManager m_memoryManager;
	MemoryAllocator* memAlloc = m_memoryManager.GetAllocator(MEMALLOC_SHM);
	LPWFSRESULT pResult = NULL;

	auto memCheck = memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
	pResult->RequestID = m.m_reqID;
	pResult->hService = m.m_hService;
	pResult->hResult = 0;
	auto ct = ::GetCurrentTime();
	GetSystemTime(&pResult->tsTimestamp);

	pResult->lpBuffer = 0;
	pResult->u.dwCommandCode = 0;

	Xfs::getInstance()->sendMessage(m.m_hWnd, WFS_OPEN_COMPLETE, (LONG)pResult);



}


void MessageHandler::process(const QueueItem & m)
{
	if (m.m_type == OPEN_QUEUE)
	{
		processOpen((QueueOpenItem&)m);
	}
	if (m.m_type == REGISTER_QUEUE)
	{
		processRegister((QueueRegisterItem&)m);
	}
	if (m.m_type == DEREGISTER_QUEUE)
	{
		processDeregister((QueueRegisterItem&)m);
	}
}

