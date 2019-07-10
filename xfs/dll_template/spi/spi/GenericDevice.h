#pragma once
#include "xfsapi.h"
#include "SharedDataStructure.h"

#include <thread>
#include <map>
#include "MemoryManager.h"

#include <deque>

#include "XfsRequest.h"

#define UNLOCKED (0)
#define LOCK_PENDING (1)
#define LOCKED (2)

using namespace std;




struct EventReg
{
	HSERVICE hService;
	DWORD events;
};


class GenericDevice
{

protected:

	map<REQUESTID, std::unique_ptr<XfsRequest>> requests;
	MemoryManager m_memoryManager;
	MemoryAllocator *memAlloc = m_memoryManager.GetAllocator(MEMALLOC_SHM);

	map<HWND, EventReg> getEventMap(HSERVICE, DWORD);
	virtual void executeProc(REQUESTID reqId) = 0;
	virtual void getInfoProc(REQUESTID reqId) = 0;
	virtual void copyLpInput(LPVOID lpData, REQUESTID reqId) = 0;



	void sendWFS_SYSE_DEVICE_STATUS(HSERVICE, DWORD);
	void sendWFS_SYSE_XXXX_ERROR(HSERVICE, DWORD, DWORD);

	void sendWFS_SYSE_VERSION_ERROR(HRESULT);
	void sendWFS_SYSE_LOCK_REQUESTED(HSERVICE);
	void sendMessage(HWND hwnd, DWORD dwEventMsg, LPARAM pResult);
private:

	volatile SharedData *sharedData = 0;
	CRITICAL_SECTION csQueue;
	CRITICAL_SECTION csRequests;

	unsigned short version = 310;
	thread *queueThread = 0;	
	int queueThreadActive = 1;



	deque< REQUESTID> deferredQueue;
	deque< REQUESTID> lockQueue;
	int isDeferredInfo(DWORD cat);
	void queueThreadProc();
	void sendRequestCompletion(REQUESTID reqId);	
	void requestCompleteHandler(REQUESTID reqId);
	
	HRESULT versionNegotiation(DWORD dwSPIVersionsRequired, DWORD dwSrvcVersionsRequired);
	void requestProc(REQUESTID);
	void enqueue(REQUESTID);
	void deregisterEvents(unsigned short hservice, HWND handle, DWORD events);
	void registerEvents(unsigned short hservice, HWND handle, DWORD events);

	void closeProc(REQUESTID reqId);
	
	void lockProc(REQUESTID reqId);
	void openProc(REQUESTID reqId);

	void unloadServiceProc(REQUESTID);
	void unlockProc(REQUESTID reqId);


	


public:
	GenericDevice(volatile SharedData* s, unsigned short version);
	~GenericDevice();

	HRESULT cancel(HSERVICE hService, REQUESTID ReqID);
	HRESULT close(HSERVICE hService, HWND hWnd, REQUESTID ReqID);
	HRESULT deregister(HSERVICE hService, DWORD dwEventClass, HWND hWndReg, HWND hWnd, REQUESTID ReqID);
	HRESULT execute(HSERVICE hService, DWORD dwCommand, LPVOID lpCmdData, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID);
	HRESULT getInfo(HSERVICE hService, DWORD dwCategory, LPVOID lpQueryDetails, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID);
	HRESULT lock(HSERVICE hService, DWORD dwTimeOut, HWND hWnd, REQUESTID ReqID);
	   
	HRESULT open(HSERVICE hService, LPSTR lpszLogicalName, HAPP hApp, LPSTR lpszAppID, DWORD dwTraceLevel, WORD dwTimeOut, HWND hWnd, REQUESTID ReqID, HPROVIDER hProvider,
		DWORD dwSPIVersionsRequired,
		LPWFSVERSION lpSPIVersion,
		DWORD dwSrvcVersionsRequired,
		LPWFSVERSION lpSrvcVersion);


	HRESULT register_(HSERVICE hService, DWORD dwEventClass, HWND hWndReg, HWND hWnd, REQUESTID ReqID);
	HRESULT setTraceLevel(HSERVICE hService, DWORD dwTraceLevel);
	HRESULT unlock(HSERVICE hService, HWND hWnd, REQUESTID ReqID);

	int canRelease();

};

