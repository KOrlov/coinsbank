// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "Logger.h"
#include <string>
#include "xfs.h"
#include "xfsapi.h"
#include <fstream>

#pragma data_seg("SHARED")  
#pragma data_seg()  
#pragma section(".shared", read, write, shared)
#define SHARED __declspec(allocate(".shared"))
SHARED volatile SharedData sharedData;

//Reading cfg file with ip:port pair
void Setup(HMODULE hModule)
{

	char filename[256];
	GetModuleFileName(hModule, filename, 256);
	
	
	Xfs::getInstance()->dllName = std::string(filename);
	string mutex_name(filename);
	size_t pos;
	pos = 0;
	while ((pos = mutex_name.find("\\", pos)) != std::string::npos)
		mutex_name.replace(pos, 1, "-"), pos += 1;
	Xfs::getInstance()->hMutex = CreateMutex(0, false, mutex_name.c_str());


	Xfs::getInstance()->l.setName(Xfs::getInstance()->dllName + ".log");
	Xfs::getInstance()->l.debug_dev("DLL_PROCESS_ATTACH Executing.");

}


void SafeSetup(HMODULE hModule)
{
	try
	{
		Setup(hModule);
	}
	catch(...)
	{
	}
}




BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{

		SafeSetup(hModule);

		
		Xfs::getInstance()->sharedData = &sharedData;
		Xfs::getInstance()->printDebugData();
		
		
		break;
	}
	case DLL_THREAD_ATTACH: break;
	case DLL_THREAD_DETACH: break;
    case DLL_PROCESS_DETACH:
	{	

		Xfs::getInstance()->l.debug_dev("detaching");
		
		
		break;
	}
        
    }
    return TRUE;
}

