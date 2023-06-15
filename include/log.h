/**
 * @file log.h
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 日志接口
 * @version 1.0
 * @date 2023-05-24
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
 * @brief 设置全局最低输出等级
 */
void SetMinLogLevel(Level::level level);

/**
 * @brief 日志接口
 */
class Log : public std::ostringstream {
 public:
  /**
   * @param level 日志等级
   * @param file_name 文件名
   * @param function_name 函数名
   * @param line_num 行号
   * @param timestamp 时间戳
   * @param logger_name 日志器名
   */
  Log(Level::level level,
      const char* file_name, 
      const char* function_name, 
      int line_num,
      uint64_t timestamp,
      std::string logger_name);
  ~Log();
 private:
  /**
   * @brief 接口的具体实现
   */
  class Impl;
  std::shared_ptr<Impl> impl_;
};

#define LOG_API_PARAM(LoggerName) \
  std::string logger_name = LoggerName, \
  const char* file_name = __builtin_FILE(), \
  const char* function_name = __builtin_FUNCTION(), \
  int line_num = __builtin_LINE(), \
  uint64_t timestamp = util::GetTimeStamp()

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