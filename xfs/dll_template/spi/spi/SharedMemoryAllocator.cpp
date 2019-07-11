//========================================================================
// SharedMemoryAllocator.cpp: implementation of the SharedMemoryAllocator class.
//========================================================================

#include "stdafx.h"
#include "xfsadmin.h"
#include "SharedMemoryAllocator.h"


LPVOID SharedMemoryAllocator_lpStartAddress;

SharedMemoryAllocator::SharedMemoryAllocator() : MemoryAllocator()
{
}

SharedMemoryAllocator::~SharedMemoryAllocator()
{

}

HRESULT SharedMemoryAllocator::AllocateBuffer(ULONG ulSize, LPVOID *lppvData)
{
	
	HRESULT result = WFMAllocateBuffer(ulSize, WFS_MEM_ZEROINIT| WFS_MEM_SHARE, lppvData);
	
	Xfs::getInstance()->l.debug("AllocateBuffer  ulSize=" + std::to_string(ulSize) + ", hResult=" + std::to_string(result)+", pointer="+std::to_string((long)lppvData));


	return result;
}

HRESULT SharedMemoryAllocator::AllocateMore(ULONG ulSize, LPVOID lpvOriginal, LPVOID * lppvData)
{
	HRESULT result = WFMAllocateMore(ulSize, lpvOriginal, lppvData);
	
	Xfs::getInstance()->l.debug("AllocateMore  ulSize=" + std::to_string(ulSize) +", hResult="+std::to_string(result));
	
	return result;
}

HRESULT SharedMemoryAllocator::FreeBuffer(LPVOID lpvData)
{
	return WFMFreeBuffer(lpvData);
}


