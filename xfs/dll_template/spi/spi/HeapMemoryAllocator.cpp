//========================================================================
// HeapMemoryAllocator.cpp: implementation of the HeapMemoryAllocator class.
//========================================================================

#include "stdafx.h"
#include "HeapMemoryAllocator.h"
#include "XFSAPI.H"
#include "XFSADMIN.H"
HeapMemoryAllocator::HeapMemoryAllocator() : MemoryAllocator()
{

}

HeapMemoryAllocator::~HeapMemoryAllocator()
{

}
/*
HRESULT HeapMemoryAllocator::AllocateBuffer(ULONG ulSize, ULONG ulFlags, LPVOID *lppvData)
{	

	*lppvData = malloc(ulSize);
	memset(*lppvData, 0, ulSize);
	return (HRESULT)lppvData;
}

HRESULT HeapMemoryAllocator::AllocateMore(ULONG ulSize, LPVOID lpvOriginal, LPVOID * lppvData)
{	

	*lppvData = malloc(ulSize);
	memset(*lppvData, 0, ulSize);
	return WFS_SUCCESS;
}

HRESULT HeapMemoryAllocator::FreeBuffer(LPVOID lpvData)
{

	if (lpvData != NULL)
		free(lpvData);
	lpvData = NULL;
	return WFS_SUCCESS;
}
*/

HRESULT HeapMemoryAllocator::AllocateBuffer(ULONG ulSize, LPVOID *lppvData)
{
	HRESULT result = WFMAllocateBuffer(ulSize, WFS_MEM_ZEROINIT, lppvData);
	return result;
}

HRESULT HeapMemoryAllocator::AllocateMore(ULONG ulSize, LPVOID lpvOriginal, LPVOID * lppvData)
{
	HRESULT result = WFMAllocateMore(ulSize, lpvOriginal, lppvData);
	return result;
}

HRESULT HeapMemoryAllocator::FreeBuffer(LPVOID lpvData)
{
	return WFMFreeBuffer(lpvData);
}