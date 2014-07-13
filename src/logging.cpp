#include "logging.h"
#include "Platform.h"
#include <inttypes.h>
#include "core/Strings.h"
#include "StackTrace.h"

using std::string;


namespace internal {

void DefaultLogHandler(LogLevel level, 
                       const char* filename, 
                       int line,
                       const string& message) 
{
    static const char* level_names[] = { "INFO", "WARNING", "ERROR", "FATAL" };

    // We use fprintf() instead of cerr because we want this to work at static
    // initialization time.
    fprintf(stderr, "[%s %s:%d] %s\n",
        level_names[level], filename, line, message.c_str());
    fflush(stderr);  // Needed on MSVC.
}

void NullLogHandler(LogLevel level, 
                    const char* filename, 
                    int line,
                    const string& message) 
{
  // Nothing.
}

static LogHandler* log_handler_ = &DefaultLogHandler;



LogMessage& LogMessage::operator<<(const string& value) 
{
    message_ += value;
    return *this;
}

LogMessage& LogMessage::operator<<(const char* value) 
{
    message_ += value;
    return *this;
}



// Since this is just for logging, we don't care if the current locale changes
// the results -- in fact, we probably prefer that.  So we use snprintf()
// instead of Simple*toa().
#undef DECLARE_STREAM_OPERATOR
#define DECLARE_STREAM_OPERATOR(TYPE, FORMAT)                       \
LogMessage& LogMessage::operator<<(TYPE value) {                    \
    /* 128 bytes should be big enough for any of the primitive */   \
    /* values which we print with this, but well use snprintf() */  \
    /* anyway to be extra safe. */                                  \
    char buffer[128];                                               \
    snprintf(buffer, sizeof(buffer), FORMAT, value);               \
    /* Guard against broken MSVC snprintf(). */                     \
    buffer[sizeof(buffer)-1] = '\0';                                \
    message_ += buffer;                                             \
    return *this;                                                   \
}

DECLARE_STREAM_OPERATOR(char         , "%c" )
DECLARE_STREAM_OPERATOR(int32_t      , "%d" )
DECLARE_STREAM_OPERATOR(uint32_t     , "%u" )
DECLARE_STREAM_OPERATOR(int64_t      , "%"PRIi64)
DECLARE_STREAM_OPERATOR(uint64_t     , "%"PRIu64)
DECLARE_STREAM_OPERATOR(double       , "%g" )
#undef DECLARE_STREAM_OPERATOR

LogMessage::LogMessage(LogLevel level, const char* filename, int line)
  : level_(level), filename_(filename), line_(line) 
{
}

LogMessage::~LogMessage() 
{
}

void LogMessage::Finish() 
{
    log_handler_(level_, filename_, line_, message_);
    if (level_ == LOGLEVEL_FATAL) 
    {
        string stack = getStackTrace();
        string msg = stringPrintf("%s[%d]: %s\nstack traceback:\n%s\n", 
            filename_, line_, message_.c_str(), stack.c_str());
        throw std::runtime_error(msg);
    }
}

void LogFinisher::operator=(LogMessage& other) 
{
    other << "\n";
    other.Finish();
}

} // namespace internal

LogHandler* SetLogHandler(LogHandler* new_func) 
{
    LogHandler* old = internal::log_handler_;
    if (old == &internal::NullLogHandler) 
    {
        old = NULL;
    }
    if (new_func == NULL) 
    {
        internal::log_handler_ = &internal::NullLogHandler;
    } 
    else 
    {
        internal::log_handler_ = new_func;
    }
    return old;
}
