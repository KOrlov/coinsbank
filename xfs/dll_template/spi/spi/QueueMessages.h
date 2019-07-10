#pragma once


#include "stdafx.h"
#include "XFSAPI.H"


#define EMPTY_QUEUE (0)
#define OPEN_QUEUE (1)
#define REGISTER_QUEUE (2)
#define DEREGISTER_QUEUE (3)
#define EXECUTE_QUEUE (4)
#define GETINFO_QUEUE (5)
#define LOCK_QUEUE (6)
#define UNLOCK_QUEUE (7)
#define CLOSE_QUEUE (8)

struct QueueItem
{
	int m_type=EMPTY_QUEUE;
	time_t  m_time;
	HSERVICE m_hService;	
	REQUESTID m_reqID;
	HWND m_hWnd;
	DWORD m_dwTimeOut;
	char m_cCancelled;
	char m_cTimeouted;
	char m_cProcessing;
};

struct QueueRegisterItem :QueueItem
{
	DWORD dwEventClass;
	HWND hWndReg;
};


struct QueueOpenItem :QueueItem
{		
	LPSTR lpszLogicalName;
	HAPP hApp;
	LPSTR lpszAppID;
	DWORD dwTraceLevel;		
	HPROVIDER hProvider;
	DWORD dwSPIVersionsRequired;
	LPWFSVERSION lpSPIVersion;
	DWORD dwSrvcVersionsRequired;
	LPWFSVERSION lpSrvcVersion;
};

struct QueueCloseItem :QueueItem
{
	
};


struct  QueueExecuteItem:QueueItem
{	
	DWORD m_dwCommand;
	LPVOID m_lpCmdData;
	DWORD m_dwTimeOut;
	
};

struct  QueueGetInfoItem :QueueItem
{
	DWORD m_dwCategory;
	LPVOID m_lpQueryDetails;
	DWORD m_dwTimeOut;	
};


struct  QueueLockItem :QueueItem
{
	
	DWORD m_dwTimeOut;	
};

struct  QueueUnlockItem :QueueItem
{		
};