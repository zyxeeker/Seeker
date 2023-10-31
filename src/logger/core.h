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
#define DEFAULT_FORMATTER_PATTERN     "%d [%P](%r){%F:%L(%N)} %m"

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
  Level::level level_;
  /**
   * @brief 文件名
   */
  const char* file_name_;
  /**
   * @brief 函数名
   */
  const char* function_name_;
  /**
   * @brief 行号
   */
  int line_num_;
  /**
   * @brief 时间戳
   */
  uint64_t timestamp_;
  /**
   * @brief 线程ID
   */
  pid_t thread_id_;
  /**
   * @brief 线程名
   */
  std::string thread_name_;
  /**
   * @brief 内容
   */
  std::ostringstream content_;
};

/**
 * @brief 日志类, 用于将事件输出至控制台和文件
 */
class Logger : public std::enable_shared_from_this<Logger> {
 public:
  using Ptr = std::shared_ptr<Logger>;

  struct Meta {
    std::string Name;
    std::string Level;
    std::string FormattingStr;
    std::vector<Outputer::Meta> Output;

    DEFINE_PROPERTIES(
      Meta,
      PROPERTY_SCHME(Name, "name"),
      PROPERTY_SCHME(Level, "level"),
      PROPERTY_SCHME(FormattingStr, "format"),
      PROPERTY_SCHME(Output, "output")
    )
  };

 public:
  Logger(std::string name, Level::level level);

  Logger(Meta meta);

  void Init();

  /**
   * @brief 输出
   */
  void Output(Event::Ptr event_ptr);
  /**
   * @brief 获取日志名
   */
  std::string name() const {
    return name_;
  }
  /**
   * @brief 设置输出等级
   */
  void set_level(Level::level level) {
    level_ = level;
  }
  /**
   * @brief 获取输出等级
   */
  Level::level level() const {
    return level_;
  }
  /**
   * @brief 设置格式管理器
   */
  void set_formatter(Formatter::Ptr formatter) {
    std::lock_guard<std::mutex> l(mutex_);
    formatter_ = formatter;
  }
  /**
   * @brief 获取格式管理器
   */
  Formatter::Ptr formatter() const {
    return formatter_;
  }
  /**
   * @brief 设置输出管理器
   */
  void set_outputer(Outputer::Ptr outputer) {
    std::lock_guard<std::mutex> l(mutex_);
    outputer_ = outputer;
  }
  /**
   * @brief 获取输出管理器
   */
  Outputer::Ptr outputer() const {
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
  Level::level level_;
  /**
   * @brief 日志格式器
   */
  Formatter::Ptr formatter_;
  /**
   * @brief 日志输出器
   */
  Outputer::Ptr outputer_;

  std::mutex mutex_;
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
   * @brief 获取指定日志器
   */
  Logger::Ptr GetLogger(std::string key);
  /**
   * @brief 添加日志器
   */
  void AddLogger(Logger::Ptr l);
  /**
   * @brief 删除指定日志器
   */
  void DeleteLogger(std::string logger_name);
  /**
   * @brief 获取默认日志器 
   */
  Logger::Ptr default_logger() {
    return default_logger_;
  }
  /**
   * @brief 获取设置的最小输出等级
   */
  Level::level min_level() const {
    return min_level_;
  }
  /**
   * @brief 设置最小输出等级
   * @param level 
   */
  void set_min_level(Level::level level) {
    min_level_ = level;
  }

 private:
  void OnCfgChanged(std::vector<Logger::Meta> cfg);

 private:
  /**
   * @brief 默认日志器
   */
  Logger::Ptr default_logger_;
  /**
   * @brief 最低输出等级
   */
  Level::level min_level_;
  /**
  * @brief 日志器字典
  */
  std::unordered_map<std::string, Logger::Ptr> loggers_;
  /**
   * @brief 从配置文件解析得到的日志器参数
   */
  std::vector<Logger::Meta> logger_meta_;

  std::mutex mutex_;
};

using Mgr = util::Single<Manager>;

} // namespace log
} // namespace seeker

#endif // _SEEKER_SRC_LOG_H_