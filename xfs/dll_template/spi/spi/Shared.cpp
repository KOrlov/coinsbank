#include "stdafx.h"
#include "Shared.h"



#pragma data_seg("SHARED")  
#pragma data_seg()  
#pragma section(".shared", read, write, shared)
#define SHARED __declspec(allocate(".shared"))
SHARED volatile int i = 0;
SHARED volatile int counter = 0;
SHARED volatile void* sharedmutex = 0;

