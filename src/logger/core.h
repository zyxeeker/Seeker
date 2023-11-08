/**
 * @file log.h
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 日志器头文件
 * @version 1.0
 * @date 2023-05-24
 */

#ifndef _SEEKER_SRC_LOG_H_
#define _SEEKER_SRC_LOG_H_

#include <mutex>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <unordered_map>

#include "../include/log.h"
#include "thread.h"

#include "cfg.hpp"
#include "util.hpp"

#include "formatter.h"
#include "output.h"


#define DEFAULT_LOGGER_NAME           "root"
#define DEFAULT_FORMATTER_PATTERN     "%d [%P](%r)[%F:%L] %m"

namespace seeker {
namespace log {

static const char* MODULE_NAME = "seeker::log";

/**
 * @brief 日志事件
 */
struct Event {
  using Ptr = std::shared_ptr<Event>;
  /**
   * @brief 等级
   */
  LEVEL Level;
  /**
   * @brief 文件名
   */
  const char* FileName;
  /**
   * @brief 函数名
   */
  const char* FunctionName;
  /**
   * @brief 行号
   */
  int Line;
  /**
   * @brief 时间戳
   */
  uint64_t Timestamp;
  /**
   * @brief 线程ID
   */
  pid_t ThreadId;
  /**
   * @brief 线程名
   */
  std::string ThreadName;
  /**
   * @brief 内容
   */
  std::ostringstream Content;
};

/**
 * @brief 日志类, 用于将事件输出至控制台和文件
 */
class Logger : public std::enable_shared_from_this<Logger> {
 public:
  using Ptr = std::shared_ptr<Logger>;

  Logger(std::string name, LEVEL level);
  Logger(LoggerDefineMeta meta);

  void Init();

  /**
   * @brief 输出
   */
  void Output(Event::Ptr event_ptr);
  /**
   * @brief 获取日志名
   */
  inline const std::string& name() const {
    return name_;
  }
  /**
   * @brief 设置输出等级
   */
  inline void set_level(LEVEL level) {
    level_ = level;
  }
  /**
   * @brief 获取输出等级
   */
  inline LEVEL level() const {
    return level_;
  }
  /**
   * @brief 设置格式管理器
   */
  inline void set_formatter(Formatter::Ptr formatter) {
    formatter_ = formatter;
  }
  /**
   * @brief 获取格式管理器
   */
  inline Formatter::Ptr formatter() const {
    return formatter_;
  }
  /**
   * @brief 设置输出管理器
   */
  inline void set_outputer(Outputer::Ptr outputer) {
    outputer_ = outputer;
  }
  /**
   * @brief 获取输出管理器
   */
  inline Outputer::Ptr outputer() const {
    return outputer_;
  }

 private:
  /**
    * @brief 日志器名字
    */
  std::string name_;
  /**
   * @brief 输出等级
   */
  LEVEL level_;
  /**
   * @brief 日志格式器
   */
  Formatter::Ptr formatter_;
  /**
   * @brief 日志输出器
   */
  Outputer::Ptr outputer_;
};

/**
 * @brief 日志管理器
 */
class Manager {
 public:
 /**
  * @brief 构建以及初始化默认日志器
  * @throw 初始化失败则抛出exception::LoggerInitError
  */
  Manager();

  /**
   * @brief 输出
   */
  void Output(const std::string& key, const Event::Ptr& event);
  /**
   * @brief 添加日志器
   */
  void AddLogger(LoggerDefineMeta&& logger);
  void AddLogger(std::vector<LoggerDefineMeta>&& loggers);
  /**
   * @brief 删除指定日志器
   */
  void DeleteLogger(const std::string& logger_name);
  /**
   * @brief 获取默认日志器 
   */
  inline const Logger::Ptr& default_logger() const {
    return default_logger_;
  }
  /**
   * @brief 获取设置的最小输出等级
   */
  LEVEL min_level() const {
    return min_level_;
  }
  /**
   * @brief 设置最小输出等级
   * @param level 
   */
  inline void set_min_level(LEVEL level) {
    min_level_ = level;
  }

 private:
  /**
   * @brief 默认日志器
   */
  Logger::Ptr default_logger_;
  /**
   * @brief 最低输出等级
   */
  LEVEL min_level_;
  /**
  * @brief 日志器字典
  */
  std::unordered_map<std::string, Logger::Ptr> loggers_;

  std::mutex mutex_;
};

using Mgr = util::Single<Manager>;

} // namespace log
} // namespace seeker

#endif // _SEEKER_SRC_LOG_H_