/*
 * timehelper.cpp
 * Author
 *     martin.wang 2014/3/3
 */

#include <time.h>
#include "timehelper.h"

#ifdef WIN32
uint32 TimeHelper::Time()
{
	return ::GetTickCount();
}

uint64 TimeHelper::TimeInMicros()
{
	return 0;
}

#elif defined(POSIX)
uint32 TimeHelper::Time()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

uint64 TimeHelper::TimeInMicros()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);

	return ts.tv_sec*(uint64)1000000 + (uint64)(ts.tv_nsec / 1000);
}

#endif

