#pragma once

class MemoryAllocator
{
public:
	virtual HRESULT AllocateBuffer(ULONG ulSize,  LPVOID *lppvData) = 0;
	virtual HRESULT AllocateMore(ULONG ulSize, LPVOID lpvOriginal, LPVOID * lppvData) = 0;
	virtual HRESULT FreeBuffer(LPVOID lpvData) = 0;
};
