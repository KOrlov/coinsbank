#pragma once

#include <queue>
#include <map>
#include <vector>
#include "xfsapi.h"
#include "thread.h"
#include "QueueMessages.h"



using namespace std;


class MessageHandler
{

private:
	Thread<MessageHandler> handler;
	void handlerProc(HANDLE e);
	void executeProc(HANDLE e);
	void timerProc(HANDLE e);

	CRITICAL_SECTION m_cs;
	CRITICAL_SECTION m_csmap;
	CRITICAL_SECTION m_vnrmap;
	CRITICAL_SECTION m_vnmap;
	CRITICAL_SECTION m_wcar; //async request
	CRITICAL_SECTION m_stl; //set trace level


	deque<QueueItem> m_queue;
	deque<QueueItem> m_deffered;


	QueueItem dequee();
	void process(const QueueItem & m);
	void processOpen(const QueueOpenItem & m);
	void processRegister(const QueueRegisterItem & m);
	void processDeregister(const QueueRegisterItem & m);
	//Bvoid processCancel(DWORD reqId);

	vector<Thread<MessageHandler>*>handlers;
	bool started = false;
public:

	MessageHandler();
	void enquee(/*const*/ QueueItem & message);
	void start();
	~MessageHandler();
};


