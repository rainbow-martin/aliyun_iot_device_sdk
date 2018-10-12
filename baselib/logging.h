/*
 * logging.h
 * Author
 *     martin.wang 2014/1/23
 * code from libjingle
 */

#ifndef __LOGGING_H__
#define __LOGGING_H__


#include "types.h"
#include <sstream>
#include <list>
#include <utility>
#include <vector>


class StreamInterface{
public:
	virtual ~StreamInterface(){}
	virtual void LogData(const void* data, size_t length) = 0;
};
//////////////////////////////////////////////////////////////////////

// Note that the non-standard LoggingSeverity aliases exist because they are
// still in broad use.  The meanings of the levels are:
//  LS_SENSITIVE: Information which should only be logged with the consent
//   of the user, due to privacy concerns.
//  LS_VERBOSE: This level is for data which we do not want to appear in the
//   normal debug log, but should appear in diagnostic logs.
//  LS_INFO: Chatty level used in debugging for all sorts of things, the default
//   in debug builds.
//  LS_WARNING: Something that may warrant investigation.
//  LS_ERROR: Something that should not have occurred.
enum LoggingSeverity { LS_SENSITIVE, LS_VERBOSE, LS_INFO, LS_WARNING, LS_ERROR, LS_TEST,
                       INFO = LS_INFO,
                       WARNING = LS_WARNING,
                       LERROR = LS_ERROR };

// LogErrorContext assists in interpreting the meaning of an error value.
//  ERRCTX_ERRNO: the value was read from global 'errno'
//  ERRCTX_HRESULT: the value is a Windows HRESULT
enum LogErrorContext { ERRCTX_NONE, ERRCTX_ERRNO, ERRCTX_HRESULT };

class LogMessage {
public:
	LogMessage(const char* file, int line, LoggingSeverity sev,
             LogErrorContext err_ctx = ERRCTX_NONE, int err = 0,
             const char* module = NULL);
	~LogMessage();

  static inline bool Loggable(LoggingSeverity sev) { return (sev >= min_sev_); }
  std::ostream& stream() { return print_stream_; }

  enum { NO_LOGGING = LS_ERROR + 1 };

  // These are attributes which apply to all logging channels
  //  LogContext: Display the file and line number of the message
  static void LogContext(int min_sev);
  //  LogThreads: Display the thread identifier of the current thread
  static void LogThreads(bool on = true);
  //  LogTimestamps: Display the elapsed time of the program
  static void LogTimestamps(bool on = true);

  // Timestamps begin with program execution, but can be reset with this
  // function for measuring the duration of an activity, or to synchronize
  // timestamps between multiple instances.
  static void ResetTimestamps();

  // These are the available logging channels
  //  Debug: Debug console on Windows, otherwise stderr
  static void LogToDebug(int min_sev);
  static int GetLogToDebug() { return dbg_sev_; }
  //  Stream: Any non-blocking stream interface.  LogMessage takes ownership of
  //   the stream.
  static void LogToStream(StreamInterface* stream, int min_sev);
  static int GetLogToStream() { return stream_sev_; }

  // Testing against MinLogSeverity allows code to avoid potentially expensive
  // logging operations by pre-checking the logging level.
  static int GetMinLogSeverity() { return min_sev_; }

	static void SetDiagnosticMode(bool f) { is_diagnostic_mode_ = f; }
	static bool IsDiagnosticMode() { return is_diagnostic_mode_; }

	static void ConfigureLog(const char* params, const char* filename);
	static void OutputToDebug(const std::string& strLogData, int sev);
	static void TerminateLogService();

protected:	
	static std::list< std::pair<StreamInterface*, int> > streamList;
	static void GetParamToken(const char* params, std::vector<std::string>& paramTable);
 private:
  // These assist in formatting some parts of the debug output.
  static const char* Describe(LoggingSeverity sev);
  static const char* DescribeFile(const char* file);

  // The ostream that buffers the formatted message before output
  std::ostringstream print_stream_;

  // The severity level of this message
  LoggingSeverity severity_;

  // String data generated in the constructor, that should be appended to
  // the message before output.
  std::string extra_;

  // dbg_sev_ and stream_sev_ are the thresholds for those output targets
  // min_sev_ is the minimum (most verbose) of those levels, and is used
  //  as a short-circuit in the logging macros to identify messages that won't
  //  be logged.
  // ctx_sev_ is the minimum level at which file context is displayed
  static int min_sev_, dbg_sev_, stream_sev_, ctx_sev_;

  // Flags for formatting options
  static bool thread_, timestamp_;

  // The timestamp at which logging started.
  static uint32 start_;

  // are we in diagnostic mode (as defined by the app)?
  static bool is_diagnostic_mode_;
};

//////////////////////////////////////////////////////////////////////
// Macros which automatically disable logging when LOGGING == 0
//////////////////////////////////////////////////////////////////////

// If LOGGING is not explicitly defined, default to enabled in debug mode
#if !defined(LOGGING)
#if defined(_DEBUG) && !defined(NDEBUG)
#define LOGGING 1
#else
#define LOGGING 1
#endif
#endif  // !defined(LOGGING)

#ifndef LOG
#if LOGGING

#ifndef ASSERT
#define ASSERT(x)	\
	if( !(x) ){		\
		LOG_F(LS_ERROR)<<"Expression: "<<#x<<" assert failure!";	\
	}
#endif

#define LOG(sev) 					\
  if(LogMessage::Loggable(sev)) 	\
    LogMessage(__FILE__, __LINE__, sev).stream()

// The _V version is for when a variable is passed in.  It doesn't do the
// namespace concatination.
#define LOG_V(sev) \
  if (LogMessage::Loggable(sev)) \
    LogMessage(__FILE__, __LINE__, sev).stream()

// The _F version prefixes the message with the current function name.
#define LOG_F(sev) LOG(sev) << __FUNCTION__ << ": "

// LOG_CHECK_LEVEL can be used as a test before performing expensive or
// sensitive operations whose sole purpose is to output logging data at the
// desired level.
#define LOG_CHECK_LEVEL(sev) \
  LogCheckLevel(sev)
#define LOG_CHECK_LEVEL_V(sev) \
  LogCheckLevel(sev)
inline bool LogCheckLevel(LoggingSeverity sev) {
  return (LogMessage::GetMinLogSeverity() <= sev);
}

// PLOG and LOG_ERR attempt to provide a string description of an errno derived
// error.  LOG_ERR reads errno directly, so care must be taken to call it before
// errno is reset.
#define PLOG(sev, err) \
  if (LogMessage::Loggable(sev)) \
    LogMessage(__FILE__, __LINE__, sev, \
                           ERRCTX_ERRNO, err).stream()
#define LOG_ERR(sev) \
  if (LogMessage::Loggable(sev)) \
    LogMessage(__FILE__, __LINE__, sev, \
                          ERRCTX_ERRNO, errno).stream()

// LOG_GLE(M) attempt to provide a string description of the HRESULT returned
// by GetLastError.  The second variant allows searching of a dll's string
// table for the error description.
#ifdef WIN32
#define LOG_GLE(sev) \
  if (LogMessage::Loggable(sev)) \
    LogMessage(__FILE__, __LINE__, sev, \
                          ERRCTX_HRESULT, GetLastError()).stream()
#define LOG_GLEM(sev, mod) \
  if (LogMessage::Loggable(sev)) \
    LogMessage(__FILE__, __LINE__, sev, \
                          ERRCTX_HRESULT, GetLastError(), mod) \
      .stream()
#endif  // WIN32


#else  // !LOGGING
#define ASSERT(x)

// Hopefully, the compiler will optimize away some of this code.
// Note: syntax of "1 ? (void)0 : LogMessage" was causing errors in g++,
//   converted to "while (false)"
#define LOG(sev) \
  while (false) LogMessage(NULL, 0, sev).stream()
#define LOG_V(sev) \
  while (false) LogMessage(NULL, 0, sev).stream()
#define LOG_F(sev) LOG(sev) << __FUNCTION__ << ": "
#define LOG_CHECK_LEVEL(sev) \
  false
#define LOG_CHECK_LEVEL_V(sev) \
  false
#define PLOG(sev, err) \
  while (false) LogMessage(NULL, 0, sev, \
                                      ERRCTX_ERRNO, 0).stream()
#define LOG_ERR(sev) \
  while (false) LogMessage(NULL, 0, sev, \
                                      ERRCTX_ERRNO, 0).stream()
#ifdef WIN32
#define LOG_GLE(sev) \
  while (false) LogMessage(NULL, 0, sev, \
                                      ERRCTX_HRESULT, 0).stream()
#define LOG_GLEM(sev, mod) \
  while (false) LogMessage(NULL, 0, sev, \
                                      ERRCTX_HRESULT, 0).stream()
#endif  // WIN32

#endif  // !LOGGING
#endif  // LOG

//////////////////////////////////////////////////////////////////////

#endif  // __LOGGING_H__

