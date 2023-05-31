/**
 * @file exception.h
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 自定义异常
 * @version 1.0
 * @date 2023-05-30
 */

#ifndef _SEEKER_SRC_EXCEPTION_H_
#define _SEEKER_SRC_EXCEPTION_H_

#include <exception>

namespace seeker {
namespace exception {

/**
 * @brief 日志初始化异常
 */
class LoggerInitError : public std::exception {
 public:
  LoggerInitError(std::string name)
    : m_buf("Logger(" + name + ") failed to initialize") {}
  const char * what () const throw () {
    return m_buf.c_str();
  }
 private:
  std::string m_buf;
};

/**
 * @brief 日志格式解析异常
 */
class LoggerParseInvalidKey : public std::exception {
 public:
  LoggerParseInvalidKey(std::string reason) 
    : m_buf(reason) {}
  const char * what () const throw () {
    return m_buf.c_str();
  }
 private:
  std::string m_buf;
};

/**
 * @brief 日志读写异常
 */
class LoggerIOError : public std::exception {
 public:
  LoggerIOError(std::string reason) 
    : m_buf(reason) {}
  const char * what () const throw () {
    return m_buf.c_str();
  }
 private:
  std::string m_buf;
};

} // exception
} // seeker

#endif // _SEEKER_SRC_EXCEPTION_H_