// http://jquery.org/license
#pragma once

#ifdef _MSC_VER
#  pragma warning(disable:4355)
// 'this' : used in base member initializer list
#endif

#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

namespace traceback {

// _________________________________________________________________________//

/**
 * @brief displays the call stack.
 *
 * @note The compiler should use the '-rdynamic', under Linux, to make this
 * feature functional
 */
class CallStack {
public:
  /**
   * @brief Default constructor
   */
  CallStack();

  /**
   * @brief Default method invoked when a CallStack is destroyed.
   */
  virtual ~CallStack() throw () {
  }

  /**
   * @brief Returns the call stack as a string
   */
  operator std::string() const;
private:
  /**
   * @brief The call stack
   */
  std::vector<std::string> stack_;
};

// _________________________________________________________________________//

/**
 * @brief Display a diagnostic message
 */
class ErrorInfo: public CallStack {
 public:
  typedef const char* (std::exception::*What)() const;

  /**
   * @brief Default constructor
   * @param object Pointer to an object of type std::exception
   * @param what Pointer to a member function of std:: exception that returns
   * the description of the exception.
   */
  ErrorInfo(std::exception* object, What what) :
      CallStack(), object_(object), what_(what), errno_(errno) {
  }

  /**
   * @brief Default method invoked when an ErrorInfo is destroyed.
   */
  virtual ~ErrorInfo() throw () {
  }

  /**
   * @brief Display the diagnostic message
   */
  virtual const char* diagnostic() const {
    std::ostringstream ss;

     ss << static_cast<std::string>(*this) << "ErrorInfo: "
        << (object_->*what_)();

    if (errno_) {
#ifdef WIN32
      char buffer[1024] = {0};
      if (!strerror_s(buffer, sizeof(buffer), errno_))
      ss << ": [Errno  " << errno_ << "] " << buffer;
#else
      ss << ": [Errno  " << errno_ << "] " << strerror(errno_);
#endif
    }
    msg_ = ss.str();
    return msg_.c_str();
  }

 private:
  mutable std::string msg_;
  std::exception* object_;
  What what_;
  int errno_;
};

// _________________________________________________________________________//

/**
 * @brief Exception class base
 */
template<class T>
class Exception: public T, public ErrorInfo {
 public:
  /**
   * @brief Default constructor
   * @param Error message
   */
  Exception(const std::string& msg) :
      T(msg), ErrorInfo(this, reinterpret_cast<ErrorInfo::What>(&T::what)) {
  }

  /**
   * @brief Default method invoked when an Exception is destroyed.
   */
  virtual ~Exception() throw () {
  }
};

// _________________________________________________________________________//

/**
 * @brief Runtime error exception
 *
 * This class defines the type of objects thrown as exceptions to report
 * errors that can only be determined during runtime.
 */
typedef Exception<std::runtime_error> RuntimeError;

// _________________________________________________________________________//

/**
 * @brief Range error exception
 *
 * This class defines the type of objects thrown as exceptions to report range
 * errors in internal computations.
 */
typedef Exception<std::range_error> RangeError;

// _________________________________________________________________________//

/**
 * @brief Overflow error exception
 *
 * This class defines the type of objects thrown as exceptions to arithmetic
 * overflow errors.
 */
typedef Exception<std::overflow_error> OverflowError;

// _________________________________________________________________________//

/**
 * @brief Underflow error exception
 *
 * This class defines the type of objects thrown as exceptions to arithmetic
 * underflow errors.
 */
typedef Exception<std::underflow_error> UnderflowError;

// _________________________________________________________________________//

/**
 * @brief Logic error exception
 *
 * This class defines the type of objects thrown as exceptions to report
 * errors in the internal logical of the program. These are theoretically
 * preventable.
 */
typedef Exception<std::logic_error> LogicError;

// _________________________________________________________________________//

/**
 * @brief Domain error exception
 *
 * This class defines the type of objects thrown as exceptions to report
 * domain errors.
 *
 * Generally, the domain of a mathematical function is the subset of values
 * that it is defined for. For example, the square root function is only
 * defined for non-negative numbers. Thus, a negative number for such a
 * function would be a domain error.
 */
typedef Exception<std::domain_error> DomainError;

// _________________________________________________________________________//

/**
 * @brief Invalid argument exception
 *
 * This class defines the type of objects thrown as exceptions to report an
 * invalid argument.
 */
typedef Exception<std::invalid_argument> InvalidArgument;

// _________________________________________________________________________//

/**
 * @brief Length error exception
 *
 * This class defines the type of objects thrown as exceptions to report a
 * length error.
 */
typedef Exception<std::length_error> LengthError;

// _________________________________________________________________________//

/**
 * @brief Out-of-range exception
 *
 * This class defines the type of objects thrown as exceptions to report an
 * out-of-range error.
 */
typedef Exception<std::out_of_range> OutOfRange;

} // namespace traceback
