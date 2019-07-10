//========================================================================
// AutoLock.cpp: implementation of the AutoLock functions.
//========================================================================
#include "stdafx.h"
#include "AutoLock.h"

AutoLock::AutoLock(CRITICAL_SECTION * plock)
{	
	m_pLock = plock;
	EnterCriticalSection(m_pLock);	
};

AutoLock::~AutoLock()
{
	if (m_pLock != NULL)
		LeaveCriticalSection(m_pLock);
};


CriticalSection::CriticalSection() { InitializeCriticalSection(&cs); }
CriticalSection::~CriticalSection() { DeleteCriticalSection(&cs); }
void CriticalSection::Enter() { EnterCriticalSection(&cs); }
BOOL CriticalSection::TryEnter() { return ::TryEnterCriticalSection(&cs); }
void CriticalSection::Leave() { LeaveCriticalSection(&cs); }