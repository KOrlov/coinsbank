#pragma once

#include <map>
#include <string>

#include "Logger.h"
#include "MessageHandler.h"
#include  "GenericDevice.h"
#include "SharedDataStructure.h"

using namespace std;



class Xfs
{
	Xfs();
	Xfs(Xfs const& copy);
	Xfs& operator=(Xfs const& copy);	
	static Xfs *instance;

	void registerEvens(unsigned short hservice, HWND handle, DWORD events);
	void deregisterEvens(unsigned short hservice, HWND handle, DWORD events);
	map<HSERVICE, std::string> servicenames;
	map<std::string, std::unique_ptr<GenericDevice>> services;



public:	
		
	static Xfs* getInstance()
	{
		if (!instance)
			instance = new Xfs();
		return instance;
	}		
	   	 
	
	std::string dllName;

	void OnExit();
	

	HRESULT cancelAsyncRequest(HSERVICE hService, REQUESTID RequestID);
	

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
	
	HRESULT unloadService();
	

	HRESULT unlock(HSERVICE hService, HWND hWnd, REQUESTID ReqID);
	




	

	
	
	volatile SharedData *sharedData;	
	Logger l;
	HANDLE hMutex = 0;

	//debug
	void printDebugData();
};

