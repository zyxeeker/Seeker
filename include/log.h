/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-05-24 16:28:41
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-31 12:46:05
 * @Description: 日志模块接口
 */

#ifndef _SEEKER_LOG_H_
#define _SEEKER_LOG_H_

#include <iostream>
#include <memory>
#include <sstream>

#include "util.h"

namespace seeker {
namespace log {

/**
 * @brief 接口定义宏
 */
#define LOG_API_PARAM_DEF                             \
  std::string logger_name,                            \
  const char* file_name,                              \
  const char* function_name,                          \
  int line_num,                                       \
  uint64_t timestamp

/**
 * @brief 接口默认参数宏
 */
#define LOG_API_PARAM(LoggerName)                     \
  std::string logger_name = LoggerName,               \
  const char* file_name = __builtin_FILE(),           \
  const char* function_name = __builtin_FUNCTION(),   \
  int line_num = __builtin_LINE(),                    \
  uint64_t timestamp = util::GetCurTimeStamp()

/**
 * @brief 日志等级
 */
struct Level {
  /**
   * @brief 等级枚举值
   */
  typedef enum {
    UNKNOWN = 0,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
  } level;
  /**
   * @brief 等级枚举值转字符串
   * @param l 枚举等级
   * @return std::string 等级字符串
   */
  static std::string ToString(level l);
  /**
   * @brief 等级字符串转等级枚举值
   * @param l_str 等级字符串
   * @return level 等级枚举值
   */
  static level FromString(std::string l_str);
};

/**
 * @brief 日志接口
 */
class Log {
 public:
  using StdOut = std::basic_ostream<char, std::char_traits<char> >;
  Log();
  ~Log();

  template<typename T>
  Log& operator<<(const T& value) {
    oss << value;
    return (*this);
  }

  Log& operator<<(StdOut& (*StdOutFuncP)(StdOut&)) {
    oss << StdOutFuncP;
    return (*this);
  }
 private:
  std::ostringstream oss;
  struct Impl;
  std::unique_ptr<Impl> impl_;
 private:
  Log(std::unique_ptr<Impl> impl);
  friend Log Debug(LOG_API_PARAM_DEF);
  friend Log Info(LOG_API_PARAM_DEF);
  friend Log Warn(LOG_API_PARAM_DEF);
  friend Log Error(LOG_API_PARAM_DEF);
  friend Log Fatal(LOG_API_PARAM_DEF);
};

/**
 * @brief 设置全局最低输出等级
 */
void SetMinLogLevel(Level::level level);

/**
 * @brief 日志Debug输出
 * @param logger_name 日志器名
 * @return Log 日志接口
 */
Log Debug(LOG_API_PARAM(""));

/**
 * @brief 日志Info输出
 * @param logger_name 日志器名
 * @return Log 日志接口
 */
Log Info(LOG_API_PARAM(""));

/**
 * @brief 日志Warn输出
 * @param logger_name 日志器名
 * @return Log 日志接口
 */
Log Warn(LOG_API_PARAM(""));

/**
 * @brief 日志Error输出
 * @param logger_name 日志器名
 * @return Log 日志接口
 */
Log Error(LOG_API_PARAM(""));

/**
 * @brief 日志Fatal输出
 * @param logger_name 日志器名
 * @return Log 日志接口
 */
Log Fatal(LOG_API_PARAM(""));

} // namespace log
} // namespace seeker

#endif // _SEEKER_LOG_H_