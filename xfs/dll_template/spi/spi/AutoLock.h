//============================================================================
// AutoLock.h: interface for the AutoLock class.
//============================================================================

#ifndef __AUTOLOCK_H__
#define __AUTOLOCK_H__

class AutoLock
{
private:
	AutoLock(const AutoLock &refAutoLock);
	AutoLock &operator=(const AutoLock &refAutoLock);

protected:
	CRITICAL_SECTION * m_pLock;

public:
	AutoLock(CRITICAL_SECTION * plock);
	~AutoLock();
};


class CriticalSection
{

public:
	class AutoLock
	{
	public:
		AutoLock(CriticalSection & critSec) : cs(critSec) { cs.Enter(); }
		virtual ~AutoLock() { cs.Leave(); }
	private:
		CriticalSection & cs;
	};
	CriticalSection();
	virtual ~CriticalSection();
	void Enter();
	BOOL TryEnter();
	void Leave();
private:
	CRITICAL_SECTION cs;
};

#endif 
