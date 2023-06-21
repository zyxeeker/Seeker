/**
 * @file exception.h
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 异常模块
 * @version 1.0
 * @date 2023-06-14
 */

#ifndef __SEEKER_EXCEPTION_H__
#define __SEEKER_EXCEPTION_H__

#include <sstream>
#include <string>
#include <exception>

namespace seeker {
/**
 * @brief 异常
 */
class Exception : public std::exception {
 public:
  Exception(std::string what_str);
  const char* what() const throw () override;
 private:
  std::string what_str_;
};

/**
 * @brief 逻辑错误
 */
class LogicError : public Exception {
 public:
  static LogicError Create(std::string module_name, 
                           std::string explanatory_str,
                           int32_t erro_num = 0);
 private:
  LogicError(std::string str);
};

/**
 * @brief 运行时错误
 */
class RunTimeError : public Exception {
 public:
  static RunTimeError Create(std::string module_name, 
                             std::string explanatory_str,
                             int32_t erro_num = 0);
 private:
  RunTimeError(std::string str);
};

/**
 * @brief 其他错误
 */
class OtherError : public Exception {
 public:
  static OtherError Create(std::string module_name, 
                           std::string explanatory_str, 
                           int32_t erro_num = 0);
 private:
  OtherError(std::string str);
};

} // seeker

#endif // __SEEKER_EXCEPTION_H__
