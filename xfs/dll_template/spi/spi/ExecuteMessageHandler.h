#pragma once

#include <queue>
#include <map>
#include <vector>
#include "xfsapi.h"
#include "thread.h"
#include "QueueMessages.h"



using namespace std;


class ExecuteMessageHandler
{

private:
	Thread<ExecuteMessageHandler> handler;	
	void handlerProc(HANDLE e);
	CRITICAL_SECTION m_cs;
	CRITICAL_SECTION m_csmap;
	CRITICAL_SECTION m_vnrmap;
	CRITICAL_SECTION m_vnmap;
	CRITICAL_SECTION m_wcar; //async request
	CRITICAL_SECTION m_stl; //set trace level

	
	queue<QueueItem> m_queue;
	queue<QueueItem> m_deferred;


	QueueItem dequee();
	void process(const QueueItem & m);
	void processOpen(const QueueOpenItem & m);
	void processRegister(const QueueRegisterItem & m);
	void processDeregister(const QueueRegisterItem & m);

	vector<Thread<ExecuteMessageHandler>*>handlers;
	bool started = false;
public:

	ExecuteMessageHandler();
	void enquee(const QueueItem & message);
	void start();	
	~ExecuteMessageHandler();
};


