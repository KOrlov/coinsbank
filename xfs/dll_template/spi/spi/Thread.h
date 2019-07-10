#pragma once

#include <Windows.h>
#include <process.h>

template<class User> class Thread
{
public:
	CRITICAL_SECTION csQueue;
	CRITICAL_SECTION csSocket;

	Thread();
	Thread(User* userThread, void(User::*funcThread)(HANDLE));
	virtual ~Thread();
	void CloseHandle();
	bool Start(User* userThread, void(User::*funThread)(HANDLE));
	bool IsStarted();
	bool Terminate(DWORD exitCode = 0);
	DWORD Suspend();
	DWORD Resume();

	bool Stop();

private:
	HANDLE handle;
	unsigned id;
	void (User::*func)(HANDLE stop);
	User* user;
	HANDLE stop;
	static unsigned __stdcall WindowsThread(void * params);

};

template <class User> Thread<User>::Thread()
{
	handle = 0;
	func = 0;
	user = 0;
	id = 0;
	stop = 0;
}

template<class User> Thread<User>::Thread(User* user, void(User::*funcThread)(HANDLE))
{
	handle = 0;
	func = 0;
	user = 0;
	stop = 0;

	Start(userThread, funcThread);
}

template<class User> Thread<User>::~Thread()
{
	if (handle != 0)
	{
		::CloseHandle(handle);
	}
	if (stop != 0)
		::CloseHandle(stop);
}

template <class User> void Thread<User>::CloseHandle()
{
	if (handle != 0)
	{
		::Closehandle(handle);
		handle = 0;
	}
}

template <class User> bool Thread<User>::Start(User * userThread, void(User::*funcThread)(HANDLE))
{
	if (IsStarted())
		return true;
	if (handle != 0)
		::CloseHandle(handle);
	if (stop == NULL)
		stop = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	else
		ResetEvent(stop);
	func = funcThread;
	user = userThread;

	handle = (HANDLE)_beginthreadex(NULL, 0, WindowsThread, this, 0, &id);
	
	return handle != 0;
}

template <class User> bool Thread<User>::IsStarted()
{
	return (handle != 0 && ::WaitForSingleObject(handle, 0) == WAIT_TIMEOUT);
}

template <class User> bool Thread<User>::Terminate(DWORD exitCode)
{
	return (::TerminateThread(handle, exitCode) == TRUE);
}

template <class User> DWORD Thread<User>::Suspend()
{
	return SuspendThread(handle);
}

template <class User> DWORD Thread<User>::Resume()
{
	return ResumeThread(handle);
}

template <class User> unsigned __stdcall Thread<User>::WindowsThread(void * param)
{
	if (param == 0)
		return 0;
	Thread * thread = (Thread *)param;
	(thread->user->*(thread->func))(thread->stop);
	_endthreadex(0);
	return 0;
}

template <class User> bool Thread<User>::Stop()
{
	::SetEvent(stop);
	return true;
}

