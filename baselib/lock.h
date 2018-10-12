/*
 * lock.h
 * 
 * Author 
 *     Martin.Wang    2012/11/27
 */
#ifndef __LOCK_H__
#define __LOCK_H__

#include "types.h"
#ifdef POSIX
#include <pthread.h>
#endif
//#include "platform.h"

/////////////////////////////////////////////////////////////////////////////
// Lockable
//
class Lockable
{
public :
	Lockable() {}
	virtual ~Lockable() {}

public :
    virtual void Lock() = 0;
    virtual void Unlock() = 0;
};

/////////////////////////////////////////////////////////////////////////////
// AutoLock
//
class AutoLock
{
public :
	AutoLock(Lockable& lock) : m_lock(lock) { lock.Lock(); }
	~AutoLock() { m_lock.Unlock(); }
private :
	Lockable& m_lock;
};

/////////////////////////////////////////////////////////////////////////////
// Locker
//
class Locker : public Lockable
{
public :
    Locker();
    virtual ~Locker();

public :
    virtual void Lock();
    virtual void Unlock();

protected :
#if defined(WIN32)
    CRITICAL_SECTION m_cs;
#elif defined(POSIX)
	pthread_mutex_t m_mutex;
#endif
};

#endif // !__LOCK_H__

