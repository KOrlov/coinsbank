#include "stdafx.h"
#include "XfsRequest.h"
#include "xfs.h"


XfsRequest::XfsRequest(REQUESTID r, DWORD t, HWND hWnd, HSERVICE hservice):timeout(t),reqId(r), hWnd(hWnd),hservice(hservice)
{	
	
	this->isCompleted = 0;
	this->isTimeouted = 0;
	this->isCancelled = 0;
	this->isExecuting = 0;
	this->isError = 0;
	if(this->timeout>0)
		setTimer();
}


XfsRequest::~XfsRequest()
{
	Xfs::getInstance()->l.debug_dev("Destructor for request="+std::to_string(reqId));
	return;
	try {
		if (timeoutThread)
			timeoutThread->detach();		
	}
	catch (...) {}

	try {
		if(timeoutThread)
			delete timeoutThread;		
	}
	catch (...) {
	}
	try {
		if (requestThread)
			requestThread->detach();
	}
	catch (...) {}

	try {
		if (requestThread)
			delete requestThread;
	}
	catch (...) {
	}

}
	


void XfsRequest::setTimer() {
	
	timeoutThread = new thread([this]() {
		DWORD t = timeout;
		while (t > 0)
		{
			if (this->isCancelled || this->isCompleted) return;
			Sleep(10);
			t -= 10;
		}
		this->isTimeouted = 1;
		if (t <1)
		{
			onTimeouted(this->reqId);
		}
	}
	);
}



