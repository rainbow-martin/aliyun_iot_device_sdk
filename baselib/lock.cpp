/*
 * Lock.cpp
 * 
 * Author 
 *     Martin.Wang    2013/10/30
 */

#include "lock.h"

#if defined(WIN32)
///////////////////////////////////////////////////////////////////////////
// Locker
Locker::Locker()
{
    InitializeCriticalSection(&m_cs);
}

Locker::~Locker()
{
    DeleteCriticalSection(&m_cs);
}

void Locker::Lock()
{
    EnterCriticalSection(&m_cs);
}

void Locker::Unlock()
{
    LeaveCriticalSection(&m_cs);
}

#elif defined(POSIX)
#include <sys/time.h>
///////////////////////////////////////////////////////////////////////////
// Locker
Locker::Locker()
{
	pthread_mutex_init(&m_mutex, NULL);
}

Locker::~Locker()
{
	pthread_mutex_destroy(&m_mutex);
}

void Locker::Lock()
{
	pthread_mutex_lock(&m_mutex);
}

void Locker::Unlock()
{
	pthread_mutex_unlock(&m_mutex);
}

#endif


