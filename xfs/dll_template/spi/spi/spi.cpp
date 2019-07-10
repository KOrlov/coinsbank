// spi.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "QueueMessages.h"
#include "xfs.h"

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif


HRESULT extern WINAPI WFPCancelAsyncRequest(HSERVICE hService, REQUESTID RequestID) //immidiate
{
	return  Xfs::getInstance()->cancelAsyncRequest(hService, RequestID);
}

HRESULT extern WINAPI WFPClose(HSERVICE hService, HWND hWnd, REQUESTID ReqID)
{
	return  Xfs::getInstance()->close(hService, hWnd, ReqID);
}

HRESULT extern WINAPI WFPDeregister(HSERVICE hService, DWORD dwEventClass,
	HWND hWndReg, HWND hWnd, REQUESTID ReqID)
{
	return  Xfs::getInstance()->deregister(hService, dwEventClass,hWndReg, hWnd, ReqID);

}

HRESULT extern WINAPI WFPExecute(HSERVICE hService, DWORD dwCommand,
	LPVOID lpCmdData, DWORD dwTimeOut, HWND hWnd,
	REQUESTID ReqID)
{
	return  Xfs::getInstance()->execute( hService,  dwCommand,lpCmdData,  dwTimeOut,  hWnd,ReqID);
}

HRESULT extern WINAPI WFPGetInfo(HSERVICE hService, DWORD dwCategory,
	LPVOID lpQueryDetails, DWORD dwTimeOut,
	HWND hWnd, REQUESTID ReqID)
{	

	return  Xfs::getInstance()->getInfo(hService, dwCategory,lpQueryDetails, dwTimeOut,hWnd, ReqID);
}

HRESULT extern WINAPI WFPLock(HSERVICE hService, DWORD dwTimeOut,
	HWND hWnd, REQUESTID ReqID)
{
	return  Xfs::getInstance()->lock( hService,  dwTimeOut,  hWnd,  ReqID);
}


HRESULT extern WINAPI WFPOpen(
	HSERVICE hService,
	LPSTR lpszLogicalName,
	HAPP hApp,
	LPSTR lpszAppID,
	DWORD dwTraceLevel,
	DWORD dwTimeOut,
	HWND hWnd,
	REQUESTID ReqID,
	HPROVIDER hProvider,
	DWORD dwSPIVersionsRequired,
	LPWFSVERSION lpSPIVersion,
	DWORD dwSrvcVersionsRequired,
	LPWFSVERSION lpSrvcVersion)
{	
	return  Xfs::getInstance()->open(
		 hService,
		 lpszLogicalName,
		 hApp,
		 lpszAppID,
		 dwTraceLevel,
		 dwTimeOut,
		 hWnd,
		 ReqID,
		 hProvider,
		 dwSPIVersionsRequired,
		 lpSPIVersion,
		 dwSrvcVersionsRequired,
		 lpSrvcVersion);

}


HRESULT extern WINAPI WFPRegister(HSERVICE hService,
	DWORD dwEventClass,
	HWND hWndReg, HWND hWnd,
	REQUESTID ReqID)
{

	return  Xfs::getInstance()->register_( hService,dwEventClass,hWndReg,  hWnd,ReqID);

}

HRESULT extern WINAPI WFPSetTraceLevel(HSERVICE hService,DWORD dwTraceLevel) //immidiate
{
	return  Xfs::getInstance()->setTraceLevel( hService, dwTraceLevel);
}

HRESULT extern WINAPI WFPUnloadService()
{
	return Xfs::getInstance()->unloadService();
}


HRESULT extern WINAPI WFPUnlock(HSERVICE hService,
	HWND hWnd,
	REQUESTID ReqID)
{

	return Xfs::getInstance()->unlock( hService,hWnd,ReqID);

}

#ifdef __cplusplus
}    /*extern "C"*/
#endif
