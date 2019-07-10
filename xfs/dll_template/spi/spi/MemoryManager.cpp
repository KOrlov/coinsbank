#include "stdafx.h"
#include "MemoryManager.h"
#include "xfsadmin.H"
#include <stdlib.h>
#include <string>

MemoryManager::MemoryManager()
{
}
MemoryAllocator* MemoryManager::GetAllocator(int inAllocType)
{
	if (inAllocType == MEMALLOC_HEAP)
	{
		return &this->m_heapMemoryAllocator;
	}
	return &this->m_sharedMemoryAllocator;
}



