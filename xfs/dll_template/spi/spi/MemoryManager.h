#pragma once
#include "HeapMemoryAllocator.h"
#include "SharedMemoryAllocator.h"
#define MEMALLOC_SHM 0
#define MEMALLOC_HEAP 1
using  namespace std;
class MemoryManager
{
private:
	HeapMemoryAllocator m_heapMemoryAllocator;
	SharedMemoryAllocator m_sharedMemoryAllocator;

public:
	MemoryManager();
	MemoryAllocator* GetAllocator(int inAllocType);
};

