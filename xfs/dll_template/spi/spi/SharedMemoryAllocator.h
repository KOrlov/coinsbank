//============================================================================
// SharedMemoryAllocator.h: interface for the SharedMemoryAllocator class.
//============================================================================
#pragma once

#include "MemoryAllocator.h"

class SharedMemoryAllocator : public MemoryAllocator
{
public:
	SharedMemoryAllocator();
	virtual ~SharedMemoryAllocator();

	virtual HRESULT AllocateBuffer(ULONG ulSize,  LPVOID *lppvData);
	virtual HRESULT AllocateMore(ULONG ulSize, LPVOID lpvOriginal, LPVOID * lppvData);
	virtual HRESULT FreeBuffer(LPVOID lpvData);
};


