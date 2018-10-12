/*
 * rollingfilelogger.h
 * Author
 *     martin.wang 2014/1/22
 */

#ifndef __ROLLING_FILE_LOGGER_H__
#define __ROLLING_FILE_LOGGER_H__

#include "logging.h"
#include "lock.h"

class RollingFileLogger : public StreamInterface{
public:
	RollingFileLogger(const char* pszFilename);
	virtual ~RollingFileLogger();

	virtual void LogData(const void * data, size_t length);

protected:
	std::string m_strFilename;
	FILE* m_pfLogFile;
	int m_nLogTick;
	uint32 m_nFileSize;

	Locker m_lock;
};


#endif

