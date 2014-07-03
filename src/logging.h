/**
 *  @file   logging.h
 *  @author ichenq@gmail.com
 *  @date   April 14, 2013
 *  @brief  logging utilities, stolen from google/protobuf/stubs/common.h
 *
 *  1) Severity Level 
 *     You can specify one of the following severity levels (in increasing order of 
 *     severity): INFO, WARNING, ERROR, and FATAL. Logging a FATAL message terminates 
 *     the program (after the message is logged) or throw an exception (if defined
 *     macro LOG_USE_EXCEPTIONS)
 *
 *  2) Conditional Logging
 *     Log a message under certain conditions, use the following macro:
 *      LOG_IF(INFO, num_cookies > 10) << "too much cookies";
 *
 *
 *  3) CHECK Macros
 *     The CHECK macro provides the ability to abort the application when a condition 
 *     is not met, similar to the assert macro defined in the standard C library.
 *      CHECK(fp->Write(x) == 4) << "Write failed!";
 *      CHECK_EQ(some_ptr, static_cast<SomeType*>(NULL));
 *      CHECK_NOTNULL(some_ptr);
 *
 *  4) Debug Mode Support
 *     Special "debug mode" logging macros only have an effect in debug mode.
 *     Use these macros to avoid slowing down your production application due to 
 *     excessive logging. 
 *       DLOG(INFO) << "Found cookies";
 *       DLOG_IF(INFO, num_cookies > 10) << "Got lots of cookies";
 *       DCHECK(num < 0) << "invalid item count";
 */

#pragma once

#include <cstdint>
#include <string>
#include <exception>
#include "traceback.h"

enum LogLevel 
{
    // Informational.  
    LOGLEVEL_INFO,

    // Warns about issues that, although not technically a
    // problem now, could cause problems in the future.  For
    // example, a // warning will be printed when parsing a
    // message that is near the message size limit.
    LOGLEVEL_WARNING,  

    // An error occurred which should never happen during
    // normal use.
    LOGLEVEL_ERROR,   

    // An error occurred from which the library cannot
    // recover.  This usually indicates a programming error
    // in the code which calls the library, especially when
    // compiled in debug mode.
    LOGLEVEL_FATAL,   

#ifdef NDEBUG
    LOGLEVEL_DFATAL = LOGLEVEL_ERROR
#else
    LOGLEVEL_DFATAL = LOGLEVEL_FATAL
#endif
};

#ifdef _WIN32
// Reslove conflict with WinGDI.h's following macro:
// #define ERROR    0
#undef ERROR
#endif  // _WIN32


namespace internal {

class LogFinisher;

class LogMessage 
{
public:
    typedef LogMessage& (*Manipulator)(LogMessage&);

    LogMessage(LogLevel level, const char* filename, int line);
    ~LogMessage();

    LogMessage& operator<<(Manipulator pfn) {return pfn(*this);}

    LogMessage& operator<<(const std::string& value);
    LogMessage& operator<<(const char* value);
    LogMessage& operator<<(char value);
    LogMessage& operator<<(int32_t value);
    LogMessage& operator<<(uint32_t value);
    LogMessage& operator<<(int64_t value);
    LogMessage& operator<<(uint64_t value);
    LogMessage& operator<<(double value);

    const std::string& GetMessage() const {return message_;}

private:
    friend class LogFinisher;
    void Finish();

    LogLevel      level_;    
    const char*   filename_;
    int           line_;
    std::string   message_;
};


// Used to make the entire "LOG(BLAH) << etc." expression have a void return
// type and print a newline after each message.
class LogFinisher 
{
 public:
  void operator=(LogMessage& other);
};

}  // namespace internal


#define LOG(LEVEL) internal::LogFinisher() =  \
    internal::LogMessage(LOGLEVEL_##LEVEL, __FILE__, __LINE__)                                           
                  
// Conditional logging
#define LOG_IF(LEVEL, CONDITION)   !(CONDITION) ? (void)0 : LOG(LEVEL)


// CHECK Macros
#define CHECK(EXPRESSION) LOG_IF(FATAL, !(EXPRESSION)) << "CHECK failed: " #EXPRESSION ": "
#define CHECK_EQ(A, B) CHECK((A) == (B))
#define CHECK_NE(A, B) CHECK((A) != (B))
#define CHECK_LT(A, B) CHECK((A) <  (B))
#define CHECK_LE(A, B) CHECK((A) <= (B))
#define CHECK_GT(A, B) CHECK((A) >  (B))
#define CHECK_GE(A, B) CHECK((A) >= (B))

namespace internal {
template<typename T>
T* CheckNotNull(const char *name, T* val) 
{
    if (val == NULL) 
    {
        LOG(FATAL) << name;
    }
    return val;
}
}  // namespace internal

#define CHECK_NOTNULL(A) \
        internal::CheckNotNull("'" #A "' must not be NULL", (A))

// Debug Mode Support
#ifdef NDEBUG

#define DLOG LOG_IF(INFO, false)

#define DCHECK(EXPRESSION) while(false) CHECK(EXPRESSION)
#define DCHECK_EQ(A, B)     DCHECK((A) == (B))
#define DCHECK_NE(A, B)     DCHECK((A) != (B))
#define DCHECK_LT(A, B)     DCHECK((A) <  (B))
#define DCHECK_LE(A, B)     DCHECK((A) <= (B))
#define DCHECK_GT(A, B)     DCHECK((A) >  (B))
#define DCHECK_GE(A, B)     DCHECK((A) >= (B))
#define DCHECK_NOTNULL

#else  // NDEBUG

#define DLOG LOG

#define DCHECK              CHECK
#define DCHECK_EQ           CHECK_EQ
#define DCHECK_NE           CHECK_NE
#define DCHECK_LT           CHECK_LT
#define DCHECK_LE           CHECK_LE
#define DCHECK_GT           CHECK_GT
#define DCHECK_GE           CHECK_GE
#define DCHECK_NOTNULL      CHECK_NOTNULL
#endif  // !NDEBUG


// Log message manipulator

typedef void LogHandler(LogLevel level, 
                        const char* filename, 
                        int line,
                        const std::string& message);

// Call SetLogHandler() to set your own handler.  This returns the old handler.  
// Set the handler to NULL to ignore log messages (but see also LogSilencer, below).
//
// Obviously, SetLogHandler is not thread-safe.  You should only call it
// at initialization time, and probably not from library code.  If you
// simply want to suppress log messages temporarily (e.g. because you
// have some code that tends to trigger them frequently and you know
// the warnings are not important to you), use the LogSilencer class
// below.
LogHandler* SetLogHandler(LogHandler* new_func);

