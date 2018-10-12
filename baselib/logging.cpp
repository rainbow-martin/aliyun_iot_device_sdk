/*
 * libjingle
 * Copyright 2004--2005, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef WIN32
#include <windows.h>
#define snprintf _snprintf
#undef ERROR  // wingdi.h
#endif

#include <iostream>
#include <iomanip>
#include <time.h>
#include <string.h>
#include <algorithm>

#ifdef POSIX
#include <pthread.h>
#endif
#include "logging.h"
#include "timehelper.h"
#include "rollingfilelogger.h"
/////////////////////////////////////////////////////////////////////////////
// Constant Labels
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// LogMessage
/////////////////////////////////////////////////////////////////////////////

#if _DEBUG
static const int LOG_DEFAULT = LS_INFO;
#else  // !_DEBUG
static const int LOG_DEFAULT = LogMessage::NO_LOGGING;
#endif  // !_DEBUG

// By default, release builds don't log, debug builds at info level
int LogMessage::min_sev_ = LOG_DEFAULT;
int LogMessage::dbg_sev_ = LOG_DEFAULT;

// No file logging by default
int LogMessage::stream_sev_ = NO_LOGGING;

// Don't bother printing context for the ubiquitous INFO log messages
int LogMessage::ctx_sev_ = LS_WARNING;

std::list< std::pair<StreamInterface*, int> > LogMessage::streamList;
// Boolean options default to false (0)
bool LogMessage::thread_, LogMessage::timestamp_;

// Program start time
uint32 LogMessage::start_ = TimeHelper::Time();

// if we're in diagnostic mode, we'll be explicitly set that way.  default to false
bool LogMessage::is_diagnostic_mode_ = false;

LogMessage::LogMessage(const char* file, int line, LoggingSeverity sev,
                       LogErrorContext err_ctx, int err, const char* module)
    : severity_(sev) {
  if (timestamp_) {
    uint32 time = TimeHelper::TimeDiff(TimeHelper::Time(), start_);
#if 0
    print_stream_ << "[" << std::setfill('0') << std::setw(3) << (time / 1000)
                  << ":" << std::setw(3) << (time % 1000) << std::setfill(' ')
                  << "] ";
sprintf( szTime, "%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d",
                     tm1.tm_year+1900, tm1.tm_mon+1, tm1.tm_mday,
                         tm1.tm_hour, tm1.tm_min,tm1.tm_sec);
#else
	time_t t = ::time(NULL);
	struct tm* tm1 = localtime(&t);
	print_stream_ << "[" << tm1->tm_year+1900 <<"-"<< std::setfill('0') << std::setw(2) << tm1->tm_mon+1
		<<"-"<< std::setw(2) << tm1->tm_mday << " " << std::setw(2) << tm1->tm_hour
		<<":" << std::setw(2) << tm1->tm_min << ":" << std::setw(2) << tm1->tm_sec
		<<"."<< std::setw(3) << time%1000 << "]";
#endif
	
  }

  if (thread_) {
#ifdef WIN32
    DWORD id = GetCurrentThreadId();
#else
	long id = pthread_self();
#endif  // WIN32
    print_stream_ << "[" << std::hex << id << std::dec << "] ";
  }

  if (severity_ >= ctx_sev_) {
    print_stream_ << Describe(sev) << "(" << DescribeFile(file)
                  << ":" << line << "): ";
  }

  if (err_ctx != ERRCTX_NONE) {
    std::ostringstream tmp;
    tmp << "[0x" << std::setfill('0') << std::hex << std::setw(8) << err << "]";
    switch (err_ctx) {
      case ERRCTX_ERRNO:
        tmp << " " << strerror(err);
        break;
  #ifdef WIN32
      case ERRCTX_HRESULT: {
        char msgbuf[256];
        DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM;
        HMODULE hmod = GetModuleHandleA(module);
        if (hmod)
          flags |= FORMAT_MESSAGE_FROM_HMODULE;
        if (DWORD len = FormatMessageA(
            flags, hmod, err,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            msgbuf, sizeof(msgbuf) / sizeof(msgbuf[0]), NULL)) {
          while ((len > 0) &&
              isspace(static_cast<unsigned char>(msgbuf[len-1]))) {
            msgbuf[--len] = 0;
          }
          tmp << " " << msgbuf;
        }
        break; }
  #endif  // WIN32
      default:
        break;
    }
    extra_ = tmp.str();
  }
}

LogMessage::~LogMessage() {
  if (!extra_.empty())
    print_stream_ << " : " << extra_;
  print_stream_ << std::endl;
  const std::string& str = print_stream_.str();

  if (severity_ >= dbg_sev_) {
    bool log_to_stderr = true;
#ifdef WIN32
    static bool debugger_present = (IsDebuggerPresent() != FALSE);
    if (debugger_present) {
      log_to_stderr = false;
      OutputDebugStringA(str.c_str());
    }
    if (log_to_stderr) {
      // This handles dynamically allocated consoles, too.
      if (HANDLE error_handle = ::GetStdHandle(STD_ERROR_HANDLE)) {
        log_to_stderr = false;
        unsigned long written;
        ::WriteFile(error_handle, str.data(), str.size(), &written, 0);
      }
    }
#endif  // WIN32
    if (log_to_stderr) {
      std::cerr << str;
      std::cerr.flush();
    }
  }

    // If write isn't fully successful, what are we going to do, log it? :)
	std::list< std::pair<StreamInterface*, int> >::iterator it;
	for(it = streamList.begin(); it != streamList.end(); it++){
		StreamInterface* pStreamInst = (StreamInterface*)it->first;
		if (severity_ >= it->second)
			pStreamInst->LogData(str.data(), str.size());
	}
}

void LogMessage::LogContext(int min_sev) {
  ctx_sev_ = min_sev;
}

void LogMessage::LogThreads(bool on) {
  thread_ = on;
}

void LogMessage::LogTimestamps(bool on) {
  timestamp_ = on;
}

void LogMessage::ResetTimestamps() {
  start_ = TimeHelper::Time();
}

void LogMessage::LogToDebug(int min_sev) {
  dbg_sev_ = min_sev;
  min_sev_ = (std::min)(dbg_sev_, stream_sev_);
}

void LogMessage::LogToStream(StreamInterface* stream, int min_sev) 
{
	if( !stream )
		return;

	streamList.push_back(std::make_pair(stream, min_sev));
}

const char* LogMessage::Describe(LoggingSeverity sev) {
  switch (sev) {
  case LS_SENSITIVE: return "Sensitive";
  case LS_VERBOSE:   return "Verbose";
  case LS_INFO:      return "Info";
  case LS_WARNING:   return "Warning";
  case LS_ERROR:     return "Error";
  default:           return "<unknown>";
  }
}

const char* LogMessage::DescribeFile(const char* file) {
  const char* end1 = ::strrchr(file, '/');
  const char* end2 = ::strrchr(file, '\\');
  if (!end1 && !end2)
    return file;
  else
    return (end1 > end2) ? end1 + 1 : end2 + 1;
}

void LogMessage::OutputToDebug(const std::string& strLogData, int sev)
{
#ifdef WIN32
	HANDLE hStdHandle = ::GetStdHandle(STD_ERROR_HANDLE);
	if(hStdHandle){
		unsigned long written = 0;
		::WriteFile(hStdHandle, strLogData.c_str(), strLogData.length(), &written, 0);
	}
#endif
}

void LogMessage::GetParamToken(const char* params, std::vector<std::string>& paramTable)
{
	if( !params )
		return;

	int nPos = 0;
	std::string strParams = params;

	while(1){
		int nFindPos = strParams.find(' ', nPos);
		if(nFindPos != std::string::npos){
			paramTable.push_back(strParams.substr(nPos, (nFindPos - nPos)));
			nPos = nFindPos+1;
		}else{
			nFindPos = strParams.length();
			paramTable.push_back(strParams.substr(nPos, (nFindPos - nPos)));
			break;
		}
	}
}

void LogMessage::ConfigureLog(const char* params,const char* filename)
{
	int nLogLevel = LS_SENSITIVE;
	std::vector<std::string> paramsList;
	GetParamToken(params, paramsList);

	for(uint32 i = 0; i < paramsList.size(); i++){
		if(paramsList[i].compare("timestamp") == 0){
			LogMessage::LogTimestamps(true);
		}else if(paramsList[i].compare("thread") == 0){
			LogMessage::LogThreads(true);
		}else if(paramsList[i].compare("sensitive") == 0){
			nLogLevel = LS_SENSITIVE;
		}else if(paramsList[i].compare("verbose") == 0){
			nLogLevel = LS_VERBOSE;
		}else if(paramsList[i].compare("info") == 0){
			nLogLevel = LS_INFO;
		}else if(paramsList[i].compare("warning") == 0){
			nLogLevel = LS_WARNING;
		}else if(paramsList[i].compare("error") == 0){
			nLogLevel = LS_ERROR;
		}
	}
	LogMessage::LogToDebug(nLogLevel);
	LogMessage::LogContext(nLogLevel);

	if( filename ){
		StreamInterface* stream = new RollingFileLogger(filename);
		LogToStream(stream, nLogLevel);
	}
}

void LogMessage::TerminateLogService()
{
	std::list< std::pair<StreamInterface*, int> >::iterator it;
	for(it = streamList.begin(); it != streamList.end(); it++){
		StreamInterface* pStreamInst = (StreamInterface*)it->first;
		if( pStreamInst ){
			delete pStreamInst;
		}
	}
	streamList.clear();
}

