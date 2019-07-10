#include "stdafx.h"
#include "xfs.h"

#include "MemoryManager.h"
#include <string>
#include "AutoMutex.h"
#include <thread>


using namespace boost::filesystem;
using namespace boost::property_tree;


using namespace std;

Xfs *Xfs::instance;
Xfs::Xfs(){}

void Xfs::OnExit()
{
	
}



HRESULT Xfs::cancelAsyncRequest(HSERVICE hService, REQUESTID RequestID)
{
	services[servicenames[hService]]->cancel(hService, RequestID);
	return 0;
}

HRESULT Xfs::close(HSERVICE hService, HWND hWnd, REQUESTID ReqID)
{	
	
	services[servicenames[hService]]->close(hService, hWnd, ReqID);
	
	return 0;
}

HRESULT Xfs::deregister(HSERVICE hService, DWORD dwEventClass,HWND hWndReg, HWND hWnd, REQUESTID ReqID)
{
	services[servicenames[hService]]->deregister(hService, dwEventClass, hWndReg, hWnd, ReqID);
	return 0;
}

HRESULT Xfs::execute(HSERVICE hService, DWORD dwCommand,	LPVOID lpCmdData, DWORD dwTimeOut, HWND hWnd,REQUESTID ReqID)
{	
	services[servicenames[hService]]->execute(hService, dwCommand, lpCmdData, dwTimeOut, hWnd, ReqID);
	return 0;
}

HRESULT Xfs::getInfo(HSERVICE hService, DWORD dwCategory,LPVOID lpQueryDetails, DWORD dwTimeOut,HWND hWnd, REQUESTID ReqID)
{	
	services[servicenames[hService]]->getInfo(hService, dwCategory, lpQueryDetails, dwTimeOut, hWnd, ReqID);

	return 0;
}

HRESULT Xfs::lock(HSERVICE hService, DWORD dwTimeOut,HWND hWnd, REQUESTID ReqID)
{	
	services[servicenames[hService]]->lock(hService, dwTimeOut, hWnd,  ReqID);

	return 0;
}


HRESULT Xfs::open(HSERVICE hService,LPSTR lpszLogicalName,HAPP hApp,LPSTR lpszAppID,DWORD dwTraceLevel,WORD dwTimeOut,HWND hWnd,REQUESTID ReqID,HPROVIDER hProvider,
	DWORD dwSPIVersionsRequired,
	LPWFSVERSION lpSPIVersion,
	DWORD dwSrvcVersionsRequired,
	LPWFSVERSION lpSrvcVersion)
{

	servicenames[hService] = std::string(lpszLogicalName);	
	if (!services[std::string(lpszLogicalName)])
	{
		ptree pt;		
		std::string configName = dllName + "." + std::string(lpszLogicalName) + ".json";
		
		boost::property_tree::read_json(configName, pt);
		
		//SPI type
		auto deviceType = pt.get<std::string>("DEVICE");
		
		//Version
		auto sVersion = pt.get<std::string>("VERSION");
		auto pos = sVersion.find('.');
		auto sMinor = sVersion.substr(0, pos);
		auto sMajor = sVersion.substr(pos + 1);

		unsigned char bMinor = atoi(sMinor.c_str());
		unsigned char bMajor = atoi(sMajor.c_str());
		unsigned short usVer = (bMajor << 8) | bMinor;


		if (deviceType == "CRT310")
		{
			auto port = pt.get<std::string>("PORT");
			auto baud = pt.get<int>("RATE");
			
			services[std::string(lpszLogicalName)] = std::make_unique<IDCCRT310>(sharedData,usVer,port, baud, configName);
		}		
		if (deviceType == "SCN83")
		{
			auto port = pt.get<std::string>("PORT");
			auto baud = pt.get<int>("RATE");

			services[std::string(lpszLogicalName)] = std::make_unique<CIMSCN83>(sharedData, usVer, port, baud, configName);
		}
		if (deviceType == "CDM4000")
		{
			auto port = pt.get<std::string>("PORT");
			auto baud = pt.get<int>("RATE");

			services[std::string(lpszLogicalName)] = std::make_unique<CDM4000>(sharedData, usVer, port, baud, configName);
		}

	}
	
	
	return services[std::string(lpszLogicalName)]->open(hService, lpszLogicalName, hApp, lpszAppID, dwTraceLevel, dwTimeOut, hWnd, ReqID, hProvider, dwSPIVersionsRequired,
		lpSPIVersion, dwSrvcVersionsRequired, lpSrvcVersion);
	
}


HRESULT Xfs::register_(HSERVICE hService,DWORD dwEventClass,	HWND hWndReg, HWND hWnd,REQUESTID ReqID)
{
	services[servicenames[hService]]->register_(hService, dwEventClass, hWndReg, hWnd, ReqID);

	return 0;
}

HRESULT Xfs::setTraceLevel(HSERVICE hService,	DWORD dwTraceLevel)
{
	return 0;
}

HRESULT Xfs::unloadService()
{
	return 0;
}


HRESULT Xfs::unlock(HSERVICE hService,	HWND hWnd,	REQUESTID ReqID)
{
	services[servicenames[hService]]->unlock(hService, hWnd, ReqID);
	return 0;
}






void Xfs::printDebugData()
{	
	EventRecord * ev = sharedData->eventRecord;
	int i = 0;
	while (ev)
	{
		l.slog("Record:" + std::to_string(i) + "hservice:" + ::to_string(ev->hService));

		ev = ev->next;
	}

}






void Xfs::registerEvens(unsigned short hservice, HWND handle, DWORD events)
{
	AutoMutex m(hMutex);

	MemoryManager m_memoryManager;
	MemoryAllocator* memAlloc = m_memoryManager.GetAllocator(MEMALLOC_SHM);
	
	if (!sharedData->eventRecord)
	{
		
		auto memCheck = memAlloc->AllocateBuffer(sizeof(EventRecord), (void **)&sharedData->eventRecord);
		sharedData->eventRecord->next = 0;
		sharedData->eventRecord->prev = 0;
		sharedData->eventRecord->events = events;
		sharedData->eventRecord->hWnd = handle;
		sharedData->eventRecord->hService = hservice;	

		return;
	}
	
	EventRecord * ev = sharedData->eventRecord;
	
	if (ev->hWnd == handle && ev->hService == hservice)
	{
		
		ev->events |= events;
		return;
	}
	while (ev->next)
	{			
		if (ev->hWnd == handle && ev->hService == hservice)
		{			
			ev->events |= events;
			return;
		}
		ev = ev->next;
	}
	
	auto memCheck = memAlloc->AllocateBuffer(sizeof(EventRecord), (void **)&ev->next);
	ev->next->prev = ev;
	ev = ev->next;
	ev->next = 0;
	ev->events = events;
	ev->hWnd = handle;
	ev->hService = hservice;	
	
}
void Xfs::deregisterEvens(unsigned short hservice, HWND handle, DWORD events)
{
	AutoMutex m(hMutex);

	MemoryManager m_memoryManager;
	MemoryAllocator* memAlloc = m_memoryManager.GetAllocator(MEMALLOC_SHM);

	if (!sharedData->eventRecord)return;
	
	EventRecord * ev = sharedData->eventRecord;

	while (ev != 0)
	{
		if (ev->hService == hservice && ev->hWnd == handle)
		{
			ev->events &= ~events;
			if (!ev->events)
			{
				if (!ev->prev&&!ev->next)
				{
					memAlloc->FreeBuffer(sharedData->eventRecord);
					sharedData->eventRecord = 0;					
					return;
				}
				if (!ev->prev)
				{
					sharedData->eventRecord = ev->next;
					ev->next->prev = sharedData->eventRecord;
					memAlloc->FreeBuffer(ev);
					return;
				}
				ev->next->prev = ev->prev;
				ev->prev->next = ev->next;
				memAlloc->FreeBuffer(ev);
				return;
			}			
		}
		ev = ev->next;
	}
}

