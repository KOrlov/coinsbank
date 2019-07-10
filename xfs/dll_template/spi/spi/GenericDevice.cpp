#include "stdafx.h"
#include "GenericDevice.h"
#include "Xfsapi.h"
#include "MemoryManager.h"
#include "AutoLock.h"
#include "AutoMutex.h"


unsigned char  getFromMajor(DWORD ver)
{
	return ver >> 16;
}
unsigned char  getFromMinor(DWORD ver)
{
	return ver >> 24;
}
unsigned char  getToMajor(DWORD ver)
{
	return ver;
}
unsigned char  getToMinor(DWORD ver)
{
	return ver >> 8;
}

void GenericDevice::sendMessage(HWND hwnd, DWORD dwEventMsg, LPARAM pResult)
{
	if (pResult)
	{
		auto ct = ::GetCurrentTime();
		GetSystemTime(&((LPWFSRESULT)pResult)->tsTimestamp);
	}
	

	Xfs::getInstance()->l.slog(
		", dwEventMsg=" + std::to_string(dwEventMsg) +
		", hService=" + std::to_string(((LPWFSRESULT)pResult)->hService) +
		", hResult=" + std::to_string(((LPWFSRESULT)pResult)->hResult) +
		", cmd=" + std::to_string(((LPWFSRESULT)pResult)->u.dwEventID)+
		", ReqId(pResult)=" + std::to_string(((LPWFSRESULT)pResult)->RequestID) +
		", PTR to pResult=" + std::to_string((long)(pResult))+
		", PTR to data=" + std::to_string((long)((LPWFSRESULT)pResult)->lpBuffer)
	
	);

	::PostMessage(hwnd, dwEventMsg, NULL, pResult);
}


void GenericDevice::sendWFS_SYSE_DEVICE_STATUS(HSERVICE hService, DWORD state)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SYSTEM_EVENT);

	for (auto i : events)
	{		
		LPWFSRESULT pResult;
		LPWFSDEVSTATUS pStatus;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		memAlloc->AllocateMore(sizeof(WFSDEVSTATUS), pResult,(void **)&pStatus);
			
		pStatus->lpszPhysicalName = 0;
		pStatus->lpszWorkstationName = 0;
		pStatus->dwState = state;
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SYSE_DEVICE_STATUS;
		pResult->lpBuffer = pStatus;
		sendMessage(i.first, WFS_SYSTEM_EVENT, (LPARAM)pResult);
		
	}
}
void GenericDevice::sendWFS_SYSE_XXXX_ERROR(HSERVICE hService,DWORD action, DWORD err)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SYSTEM_EVENT);

	for (auto i : events)
	{
		
		LPWFSRESULT pResult;
		LPWFSHWERROR pError;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
		memAlloc->AllocateMore(sizeof(WFSHWERROR), pResult, (void **)&pError);

		pError->lpszPhysicalName = 0;
		pError->lpszWorkstationName = 0;
		pError->lpszLogicalName = 0;
		pError->lpbDescription = 0;
		pError->lpszAppID = 0;
		pError->dwSize = 0;
		pError->dwAction = action;

		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = err;
		pResult->lpBuffer = pError;
		sendMessage(i.first, WFS_SYSTEM_EVENT, (LPARAM)pResult);
		
	}
}
void GenericDevice::sendWFS_SYSE_VERSION_ERROR(HRESULT res)
{
	map<HWND, EventReg> events = getEventMap(0, WFS_SYSTEM_EVENT);

	for (auto i : events)
	{
		
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);

		LPWFSVRSNERROR pVersionError;
		memAlloc->AllocateMore(sizeof(WFSVRSNERROR), pResult, (void **)&pVersionError);
		memAlloc->AllocateMore(sizeof(WFSVERSION), pResult, (void **)&pVersionError->lpWFSVersion);
			


		pResult->hService = 0;
		pResult->hResult = res;
		pResult->u.dwEventID = WFS_SYSE_VERSION_ERROR;
		pResult->lpBuffer = pVersionError;
		sendMessage(i.first, WFS_SYSTEM_EVENT, (LPARAM)pResult);
		
	}
}
void GenericDevice::sendWFS_SYSE_LOCK_REQUESTED(HSERVICE hService)
{
	map<HWND, EventReg> events = getEventMap(this->sharedData->lockedByHservice, WFS_SYSTEM_EVENT);

	for (auto i : events)
	{
		LPWFSRESULT pResult;
		memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&pResult);
			
		pResult->hService = hService;
		pResult->hResult = 0;
		pResult->u.dwEventID = WFS_SYSE_LOCK_REQUESTED;
		pResult->lpBuffer = 0;
		sendMessage(i.first, WFS_SYSTEM_EVENT, (LPARAM)pResult);
		
	}
}

int GenericDevice::isDeferredInfo(DWORD cat)
{
	return 0;
}

int GenericDevice::canRelease()
{
	return 0;
}


HRESULT GenericDevice::versionNegotiation(DWORD dwSPIVersionsRequired,	DWORD dwSrvcVersionsRequired)
{
	if (!(getFromMajor(dwSPIVersionsRequired) <= getToMajor(version))) return WFS_ERR_SPI_VER_TOO_LOW;
	if (!(getFromMajor(dwSrvcVersionsRequired) <= getToMajor(version))) return WFS_ERR_SPI_VER_TOO_LOW;
	
	if (!(getToMajor(dwSPIVersionsRequired) >= getToMajor(version))) return WFS_ERR_SPI_VER_TOO_HIGH;
	if (!(getToMajor(dwSrvcVersionsRequired) >= getToMajor(version))) return WFS_ERR_SPI_VER_TOO_HIGH;

	if (!(getFromMinor(dwSPIVersionsRequired) <= getToMinor(version))) return WFS_ERR_SPI_VER_TOO_LOW;
	if (!(getFromMinor(dwSrvcVersionsRequired) <= getToMinor(version))) return WFS_ERR_SPI_VER_TOO_LOW;

	if (!(getToMinor(dwSPIVersionsRequired) >= getToMinor(version))) return WFS_ERR_SPI_VER_TOO_HIGH;
	if (!(getToMinor(dwSrvcVersionsRequired) >= getToMinor(version))) return WFS_ERR_SPI_VER_TOO_HIGH;

		

	return WFS_SUCCESS;
}


void GenericDevice::sendRequestCompletion(REQUESTID reqId)
{
	//Xfs::getInstance()->l.slog("LP BUFFER AT END=" + std::to_string((long)(requests[reqId]->pResult->lpBuffer)));


	sendMessage(requests[reqId]->hWnd, requests[reqId]->completionEvent, (LPARAM)requests[reqId]->pResult);
}



void GenericDevice::requestCompleteHandler(REQUESTID reqId)
{	
	Xfs::getInstance()->l.slog("requestCompleteHandler, reqId=" + std::to_string(reqId));

	this->requests[reqId]->isCompleted = 1;
	requests[reqId]->pResult->hService = requests[reqId]->hservice;
	requests[reqId]->pResult->RequestID = reqId;
	
	auto ct = ::GetCurrentTime();
	GetSystemTime(&requests[reqId]->pResult->tsTimestamp);
	
	this->sendRequestCompletion(reqId);

	if (this->requests[reqId]->lpParam)
	{
		Xfs::getInstance()->l.slog("requestCompleteHandler free lpParam, reqId=" + std::to_string(reqId));

		memAlloc->FreeBuffer(this->requests[reqId]->lpParam);
	}
	
		
	AutoLock lock(&csRequests);
		this->requests.erase(reqId);
}


HRESULT GenericDevice::close(HSERVICE hService, HWND hWnd, REQUESTID ReqID) {

	AutoLock lock(&csRequests);

	requests[ReqID] = std::make_unique<XfsRequest>(ReqID, 0,hWnd, hService);
	requests[ReqID]->completionEvent = WFS_CLOSE_COMPLETE;
	requests[ReqID]->requestThread = 0;
	requests[ReqID]->messageType = MSG_CLOSE;

	enqueue(ReqID);

return 0;
}

HRESULT GenericDevice::deregister(HSERVICE hService, DWORD dwEventClass, HWND hWndReg, HWND hWnd, REQUESTID ReqID) {
	
	AutoLock lock(&csRequests);

	this->deregisterEvents(hService, hWndReg, dwEventClass);
	requests[ReqID] = std::make_unique<XfsRequest>(ReqID, 0, hWnd, hService);
	requests[ReqID]->messageType = MSG_DEREGISTER;
	requests[ReqID]->completionEvent = WFS_DEREGISTER_COMPLETE;
	requests[ReqID]->requestThread = new thread(&GenericDevice::requestProc, this, ReqID);
	
	return 0; 
}
HRESULT GenericDevice::execute(HSERVICE hService, DWORD dwCommand, LPVOID lpCmdData, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID) { 
	
	AutoLock lock(&csRequests);

	Xfs::getInstance()->l.slog("GenericDevice::execute command=" + std::to_string(dwCommand)+ "lpCmdData="+std::to_string((long)lpCmdData));
	requests[ReqID] = std::make_unique<XfsRequest>(ReqID, dwTimeOut, hWnd, hService);
	requests[ReqID]->cmd = dwCommand;
	//requests[ReqID]->lpParam = lpCmdData;
	requests[ReqID]->messageType = MSG_EXECUTE;
	requests[ReqID]->completionEvent = WFS_EXECUTE_COMPLETE;
		
	copyLpInput(lpCmdData, ReqID);
	enqueue(ReqID);
	

	return 0; 

}
HRESULT GenericDevice::getInfo(HSERVICE hService, DWORD dwCategory, LPVOID lpQueryDetails, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID) { 
	
	AutoLock lock(&csRequests);

	requests[ReqID] = std::make_unique<XfsRequest>(ReqID, dwTimeOut, hWnd, hService);
	requests[ReqID]->completionEvent = WFS_GETINFO_COMPLETE;
	requests[ReqID]->requestThread = 0;
	requests[ReqID]->messageType = MSG_GETINFO;
	requests[ReqID]->cmd = dwCategory;
	//requests[ReqID]->lpParam = lpQueryDetails;
	requests[ReqID]->reqId = ReqID;

	Xfs::getInstance()->l.slog("getInfo, reqId=" + std::to_string(requests[ReqID]->reqId) + ", category=" + std::to_string(dwCategory));

	copyLpInput(lpQueryDetails, ReqID);

	

	
	if (isDeferredInfo(dwCategory))
	{
		AutoLock lock(&csQueue);
		deferredQueue.push_back(ReqID);
	}
	else
	{
		requests[ReqID]->requestThread = new thread(&GenericDevice::requestProc, this, ReqID);
	}
	
	return 0; 


}
HRESULT GenericDevice::lock(HSERVICE hService, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID) { 
	
	AutoLock lock(&csRequests);

	requests[ReqID]  = std::make_unique<XfsRequest>(ReqID, dwTimeOut, hWnd, hService);
	requests[ReqID]->completionEvent = WFS_LOCK_COMPLETE;
	requests[ReqID]->requestThread = 0;
	requests[ReqID]->messageType = MSG_LOCK;
	
	
	enqueue(ReqID);	
	return 0; 
}

HRESULT GenericDevice::cancel(HSERVICE hService,  REQUESTID ReqID) { 
	if (ReqID)
	{
		requests[ReqID]->isCancelled = 1;
		if (!requests[ReqID]->isExecuting)
		{
			requests[ReqID]->requestThread=new thread(&GenericDevice::requestProc, this, ReqID);
		}
	}		
	else
	{
		for (auto&& r : requests)
		{
			if (hService == r.second->hservice)
			{
				r.second->isCancelled = 1;
				if (!r.second->isExecuting)
				{
					r.second->requestThread= new thread(&GenericDevice::requestProc, this, r.first);
				}
			}
				
		}
	}
	return 0; 
}

HRESULT GenericDevice::open(HSERVICE hService, LPSTR lpszLogicalName, HAPP hApp, LPSTR lpszAppID, DWORD dwTraceLevel, WORD dwTimeOut, HWND hWnd, REQUESTID ReqID, HPROVIDER hProvider,
DWORD dwSPIVersionsRequired,
	LPWFSVERSION lpSPIVersion,
	DWORD dwSrvcVersionsRequired,
	LPWFSVERSION lpSrvcVersion) {

	HRESULT res = this->versionNegotiation(dwSPIVersionsRequired, dwSrvcVersionsRequired);
	if (res)
	{
		return res;
	}	
	else
	{
		lpSPIVersion->szDescription[0] = 0;
		lpSPIVersion->szSystemStatus[0] = 0;
		lpSPIVersion->wHighVersion = this->version;
		lpSPIVersion->wLowVersion = this->version;
		lpSPIVersion->wVersion = version;
			
		lpSrvcVersion->wHighVersion = this->version;
		lpSrvcVersion->wLowVersion = this->version;
		lpSrvcVersion->wVersion = version;
		lpSrvcVersion->szDescription[0] = 0;
		lpSrvcVersion->szSystemStatus[0] = 0;
	}
	
	AutoLock lock(&csRequests);

	requests[ReqID] = std::make_unique<XfsRequest>(ReqID, dwTimeOut, hWnd, hService);
	requests[ReqID]->completionEvent = WFS_OPEN_COMPLETE;
	requests[ReqID]->onTimeouted.connect(boost::bind((&GenericDevice::requestProc), this, ReqID));
	requests[ReqID]->requestThread = new thread(&GenericDevice::requestProc, this, ReqID);
	requests[ReqID]->messageType = MSG_OPEN;

	

	return res;
}	

void GenericDevice::requestProc(REQUESTID ReqID) {
		
	this->requests[ReqID]->onTimeouted.disconnect_all_slots();
	auto memCheck = memAlloc->AllocateBuffer(sizeof(WFSRESULT), (void **)&requests[ReqID]->pResult);	
	
	Xfs::getInstance()->l.slog("REQUEST="+std::to_string(ReqID)+"PRESULT=" + std::to_string((long)requests[ReqID]->pResult));


	memset(requests[ReqID]->pResult, 0, sizeof(WFSRESULT));

	if (requests[ReqID]->messageType == MSG_EXECUTE || requests[ReqID]->messageType == MSG_GETINFO)
	{
		requests[ReqID]->pResult->u.dwCommandCode = requests[ReqID]->cmd;
	}

	if (requests[ReqID]->isError)
	{
		requests[ReqID]->pResult->hResult = requests[ReqID]->hResult;		
	}
	else
	if (requests[ReqID]->isCancelled)
	{
		requests[ReqID]->pResult->hResult = WFS_ERR_CANCELED;
	}
	else
	if (requests[ReqID]->isTimeouted)
	{
		requests[ReqID]->pResult->hResult = WFS_ERR_TIMEOUT;
	}
	else
	{
		requests[ReqID]->isExecuting = 1;
		requests[ReqID]->pResult->hResult = 0;
		if (requests[ReqID]->messageType == MSG_LOCK)
		{
			Xfs::getInstance()->l.slog("Request processing: LOCK");
			requests[ReqID]->completionEvent = WFS_LOCK_COMPLETE;
			lockProc(ReqID);
		}
		else
		if (requests[ReqID]->messageType == MSG_UNLOCK)
		{
			Xfs::getInstance()->l.slog("Request processing: UNLOCK");
			requests[ReqID]->completionEvent = WFS_UNLOCK_COMPLETE;
			unlockProc(ReqID);
		}
		else
		if (requests[ReqID]->messageType == MSG_EXECUTE)
		{
			Xfs::getInstance()->l.slog("Request processing: EXECUTE");
			requests[ReqID]->completionEvent = WFS_EXECUTE_COMPLETE;
			executeProc(ReqID);
		}
		else
		if (requests[ReqID]->messageType == MSG_GETINFO)
		{
			Xfs::getInstance()->l.slog("Request processing: GETINFO");
			requests[ReqID]->completionEvent = WFS_GETINFO_COMPLETE;
			requests[ReqID]->pResult->u.dwCommandCode = requests[ReqID]->cmd;
			getInfoProc(ReqID);
		}
		else
		if (requests[ReqID]->messageType == MSG_CLOSE)
		{
			Xfs::getInstance()->l.slog("Request processing: CLOSE");
			requests[ReqID]->completionEvent = WFS_CLOSE_COMPLETE;
			
			requests[ReqID]->pResult->u.dwCommandCode;
			closeProc(ReqID);
		}
	}

	requestCompleteHandler(ReqID);

}

HRESULT GenericDevice::register_(HSERVICE hService, DWORD dwEventClass, HWND hWndReg, HWND hWnd, REQUESTID ReqID) {
	
	this->registerEvents(hService, hWndReg, dwEventClass);
	AutoLock lock(&csRequests);

	requests[ReqID] = std::make_unique<XfsRequest>(ReqID, 0, hWnd, hService);
	requests[ReqID]->completionEvent = WFS_REGISTER_COMPLETE;
	requests[ReqID]->messageType = MSG_REGISTER;
	requests[ReqID]->requestThread = new thread(&GenericDevice::requestProc, this, ReqID);
	return 0;
}
HRESULT GenericDevice::setTraceLevel(HSERVICE hService, DWORD dwTraceLevel) { 
	
	return 0; 
}

HRESULT GenericDevice::unlock(HSERVICE hService, HWND hWnd, REQUESTID ReqID) {
	
	AutoLock lock(&csRequests);

	requests[ReqID] = std::make_unique<XfsRequest>(ReqID, 0, hWnd, hService);
	requests[ReqID]->completionEvent = WFS_UNLOCK_COMPLETE;
	requests[ReqID]->requestThread = 0;
	requests[ReqID]->messageType = MSG_UNLOCK;
	enqueue(ReqID);
	return 0; 
}

void GenericDevice::enqueue(REQUESTID ReqID)
{
	requests[ReqID]->onTimeouted.connect(boost::bind((&GenericDevice::requestProc), this, ReqID));

	AutoLock lock(&csQueue);
	
		if (requests[ReqID]->messageType == MSG_EXECUTE)
		{
		if (sharedData->serviceState == UNLOCKED || sharedData->serviceState == LOCK_PENDING)
		{
			deferredQueue.push_back(ReqID);
		}
		else
			if (sharedData->serviceState == LOCKED)
			{
				if (sharedData->lockedByHservice == requests[ReqID]->hservice)
				{
					deferredQueue.push_back(ReqID);
				}
				else
				{
					requests[ReqID]->isError = 1;
					requests[ReqID]->hResult = WFS_ERR_LOCKED;
					requests[ReqID]->requestThread = new thread(&GenericDevice::requestProc, this, ReqID);
				}
			}
	}
	else
	if (requests[ReqID]->messageType == MSG_LOCK)
	{
		if (sharedData->serviceState == LOCKED && requests[ReqID]->hservice!= sharedData->lockedByHservice)
		{
			sendWFS_SYSE_LOCK_REQUESTED(requests[ReqID]->hservice);
		}
		deferredQueue.push_back(ReqID);
	}
	else
		deferredQueue.push_back(ReqID);		
}

void GenericDevice::queueThreadProc() {
	while (queueThreadActive)
	
	{	
		int processRequest = 1;
		REQUESTID r;
		{
			AutoLock lock(&csQueue);
			if (this->sharedData->serviceState == UNLOCKED && !lockQueue.empty())
			{
				r = lockQueue.front();
				lockQueue.pop_front();
			}
			else
			{
				if (deferredQueue.empty())
				{					
					Sleep(1);
					continue;
				}
					
				r = deferredQueue.front();
				deferredQueue.pop_front();
				if (requests[r]->messageType == MSG_LOCK)
				{
					if (this->sharedData->serviceState == LOCKED && this->sharedData->lockedByHservice != requests[r]->hservice)
					{
						processRequest = 0;
						lockQueue.push_back(r);
					}
				}
			}
			
		}
		if (processRequest) {

			if (requests.count(r))			
				requestProc(r);

		}
		
		
		
	}
	

}


void GenericDevice::registerEvents(unsigned short hservice, HWND handle, DWORD events)
{	
	AutoMutex m(Xfs::getInstance()->hMutex);

	Xfs::getInstance()->l.slog("Request processing: REGISTER EVENTS");
	
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

map<HWND, EventReg> GenericDevice::getEventMap(HSERVICE hService, DWORD dwEvent)
{
	AutoMutex m(Xfs::getInstance()->hMutex);

	map<HWND, EventReg> res;
	EventRecord * ev = sharedData->eventRecord;

	while (ev)
	{
		if ((ev->hService == hService || !hService) && ev->events&dwEvent)
		{
			res[ev->hWnd].events = ev->events;
			res[ev->hWnd].hService = ev->hService;
		}
		
		ev = ev->next;
	}
	return res;
}


void GenericDevice::deregisterEvents(unsigned short hservice, HWND handle, DWORD events)
{
	
	AutoMutex m(Xfs::getInstance()->hMutex);

	Xfs::getInstance()->l.slog("Request processing: DEREGISTER EVENTS");


	if (!sharedData->eventRecord)return;

	EventRecord * ev = sharedData->eventRecord;

	while (ev != 0)
	{
		if (ev->hService == hservice && (ev->hWnd == handle || !handle))
		{
			ev->events &= ~events;
			if (!ev->events)
			{
				if (!ev->prev && !ev->next)
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


//WFP HANDLERS 





void GenericDevice::closeProc(REQUESTID reqId)
{
	if (sharedData->serviceState == LOCKED || sharedData->serviceState == LOCK_PENDING)
	{
		if (sharedData->lockedByHservice == requests[reqId]->hservice)
		{
			sharedData->serviceState = UNLOCKED;
			sharedData->lockedByHservice = 0;
		}
	}
	deregisterEvents(requests[reqId]->hservice, 0, SERVICE_EVENTS | USER_EVENTS | SYSTEM_EVENTS | EXECUTE_EVENTS);

}


//exec
//getinfo


void GenericDevice::lockProc(REQUESTID reqId)
{
	sharedData->serviceState = LOCKED;
	sharedData->lockedByHservice = requests[reqId]->hservice;

}

void GenericDevice::openProc(REQUESTID reqId)
{}


void GenericDevice::unloadServiceProc(REQUESTID)
{
}

void GenericDevice::unlockProc(REQUESTID reqId)
{
	if (sharedData->serviceState == LOCKED && sharedData->lockedByHservice == requests[reqId]->hservice)
	{
		sharedData->serviceState = UNLOCKED;
		sharedData->lockedByHservice = 0;
	}
	else
	{
		requests[reqId]->pResult->hResult = WFS_ERR_NOT_LOCKED;
	}
}




//CONSTRUCTOR & ~

GenericDevice::GenericDevice(volatile SharedData* s, unsigned short version)
{
	this->sharedData = s;
	this->version = version;

	Xfs::getInstance()->l.slog("MY SHARED COUNTERP:" + std::to_string(this->sharedData->count));

	memAlloc = m_memoryManager.GetAllocator(MEMALLOC_SHM);

	if (!this->sharedData)
		this->sharedData->serviceState = UNLOCKED;

	this->sharedData->count++;

	InitializeCriticalSection(&csQueue);
	InitializeCriticalSection(&csRequests);
	
	queueThread = new thread(&GenericDevice::queueThreadProc, this);

}


GenericDevice::~GenericDevice()
{
	this->sharedData->count--;
	if (!this->sharedData->count)
	{
	}

	queueThreadActive = 0;
	queueThread->join();
	delete queueThread;
	DeleteCriticalSection(&csQueue);
	DeleteCriticalSection(&csRequests);

}

