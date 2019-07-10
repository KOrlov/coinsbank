#pragma once
#include "xfsapi.h"

struct EventRecord
{
	HSERVICE hService;
	HWND hWnd;
	DWORD events;
	EventRecord* next;
	EventRecord* prev;
};

struct SharedData
{
	int count;
	HSERVICE lockedByHservice;
	DWORD serviceState;
	EventRecord* eventRecord;
};