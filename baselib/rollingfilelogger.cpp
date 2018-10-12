/*
 * rollingfilelogger.cpp
 * Author
 *     martin.wang 2014/1/23
 */

#include "rollingfilelogger.h"
#include "timehelper.h"
#include <stdio.h>

#define MAX_LOG_FILE_SIZE 2*1024*1024

RollingFileLogger::RollingFileLogger(const char* pszFilename)
:m_pfLogFile(NULL)
{
	m_nFileSize = 0;
	if(pszFilename){
		m_strFilename = pszFilename;
		m_pfLogFile = fopen(pszFilename, "a+b");

		if( m_pfLogFile ){
			int nResult = fseek(m_pfLogFile, 0, SEEK_END);
			if( nResult == 0 ){
				m_nFileSize = ::ftell(m_pfLogFile);
			}else{
				::fclose(m_pfLogFile);
				::remove(m_strFilename.c_str());
				m_pfLogFile = fopen(pszFilename, "a+b");
			}
		}
	}
	m_nLogTick = 0;
}

RollingFileLogger::~RollingFileLogger()
{
	if(m_pfLogFile)
		::fclose(m_pfLogFile);
}

void RollingFileLogger::LogData(const void * data,size_t length)
{
	if( m_pfLogFile ){
		AutoLock locker(m_lock);
		::fwrite(data, 1, length, m_pfLogFile);
		m_nFileSize += length;

		if( TimeHelper::Time() - m_nLogTick > 5000 )
			::fflush(m_pfLogFile);
		m_nLogTick = TimeHelper::Time();

		if( m_nFileSize > MAX_LOG_FILE_SIZE ){
			std::string strOldFile = m_strFilename + ".old";
			::fclose(m_pfLogFile);

			// 1. remove old file
			::remove(strOldFile.c_str());

			// 2. rename log file, if failed, skip log writting
			if( ::rename(m_strFilename.c_str(), strOldFile.c_str()) == 0 ){
				m_pfLogFile = fopen(m_strFilename.c_str(), "a+b");
				m_nFileSize = 0;
			}
		}
	}
}

