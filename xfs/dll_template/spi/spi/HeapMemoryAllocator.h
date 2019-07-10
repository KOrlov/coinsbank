#pragma once

#include "MemoryAllocator.h"

class HeapMemoryAllocator : public MemoryAllocator
{
public:
	HeapMemoryAllocator();
	virtual ~HeapMemoryAllocator();

	virtual HRESULT AllocateBuffer(ULONG ulSize, LPVOID *lppvData);
	virtual HRESULT AllocateMore(ULONG ulSize, LPVOID lpvOriginal, LPVOID * lppvData);
	virtual HRESULT FreeBuffer(LPVOID lpvData);
};

