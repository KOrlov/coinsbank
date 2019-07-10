#include "xfsapi.h"
#include <thread>
#include <atomic>
#include <boost/signals2.hpp>

#define MSG_OPEN (1)
#define MSG_REGISTER (2)
#define MSG_DEREGISTER (3)
#define MSG_EXECUTE (4)
#define MSG_GETINFO (5)
#define MSG_LOCK (6)
#define MSG_UNLOCK (7)
#define MSG_CLOSE (8)


#pragma once
using namespace std;
using namespace boost::signals2;

class XfsRequest  {
	
private:
	thread *timeoutThread = 0;		

public:	

	LPVOID lpParam = 0; 
	int messageType = 0;
	DWORD cmd = 0;
	LPWFSRESULT pResult = 0;
	boost::signals2::signal<void(REQUESTID)> onTimeouted;
	DWORD completionEvent = 0;
	REQUESTID reqId = 0;
	DWORD   timeout = 0;
	HWND hWnd = 0;
	HSERVICE hservice = 0;
	thread * requestThread = 0;
	void setTimer();

	std::atomic<char> isCompleted;
	std::atomic<char> isTimeouted;
	std::atomic<char> isCancelled;
	std::atomic<char> isExecuting;
	std::atomic<char> isError;
	HRESULT hResult;

	XfsRequest() = default;
	XfsRequest(REQUESTID reqId, DWORD timeout, HWND hWnd, HSERVICE hservice);
	~XfsRequest();
};

