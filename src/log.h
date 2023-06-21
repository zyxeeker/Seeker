/**
 * @file log.h
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 日志器头文件
 * @version 1.0
 * @date 2023-05-24
 */

#ifndef _SEEKER_SRC_LOG_H_
#define _SEEKER_SRC_LOG_H_

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <unordered_map>
#include "../include/log.h"
#include "../include/cfg.h"
#include "../include/util.h"
#include "thread.h"

#define DEFAULT_LOGGER_NAME           "root"
#define DEFAULT_FORMATTER_PATTERN     "%d [%P](%r){%F:%L(%N)} %m"
#define DEFAULT_DATETIME_PATTERN      "%Y-%m-%d %H:%M"
#define EMPTY_PARAM                   ""

namespace seeker {
namespace log {
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
  TID thread_id_;
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
 * @brief 日志格式管理类, 用于解析格式和存储解析后各子项的顺序
 */
class FormattingMgr {
 public:
  using Ptr = std::shared_ptr<FormattingMgr>;
  FormattingMgr(std::string format_str) 
    : raw_(std::move(format_str)) {}
  /**
   * @brief 初始化对格式进行解析
   * @throw 不成功则抛出异常exception::ParseInvalidKey
   */
  void Init();
  /**
   * @brief 返回格式字符串
   * @return std::string 
   */
  std::string raw() const {
    return raw_;
  }

  /**
   * @brief 格式参数项接口(子模块)
   */
  class IItem {
   public:
    using Ptr = std::shared_ptr<IItem>;
    virtual ~IItem() {};
    /**
     * @brief 将数据进行转换并输出到传入的流中
     * @param ss 流
     * @param e 事件
     */
    virtual void ToStream(std::ostream &os, 
                          const std::string& logger_name, 
                          Event::Ptr event_ptr) = 0;
  };
  /**
   * @brief 添加子项
   */
  void AddItem(IItem::Ptr item) {
    item_arr_.push_back(item);
  }
  /**
   * @brief 清空子项数组
   */
  void ClearItemArr() {
    item_arr_.clear();
  }
  /**
   * @brief 返回子项数组
   * @return const std::vector<IItem::Ptr>& 
   */
  const std::vector<IItem::Ptr>& item_arr() const {
    return item_arr_;
  }
 private:
  /**
   * @brief 原始格式字符串
   */
  std::string raw_;
  /**
   * @brief 解析后的格式子项数组
   */
  std::vector<IItem::Ptr> item_arr_;
};

/**
 * @brief 输出类型
 */
enum OUTPUT_TYPE {
  OUTPUT_STD = 0,
  OUTPUT_FILE,
  OUTPUT_NUM
};

/**
 * @brief 日志输出管理类
 */
class OutputMgr {
 public:
  using Ptr = std::shared_ptr<OutputMgr>;
  /**
   * @brief 日志输出接口(子模块)
   */
  class IOutput {
  public:
    using Ptr = std::shared_ptr<IOutput>;
    virtual ~IOutput() {}
    /**
     * @brief 输出接口
     * @param buf 传入的字符串数据
     */
    virtual void Output(const std::string& logger_name,
                        const std::vector<FormattingMgr::IItem::Ptr>& items,
                        const Event::Ptr event_ptr) = 0;
  };
  /**
   * @brief 添加输出
   */
  void AddOutput(IOutput::Ptr output);
  /**
   * @brief 清空输出数组
   */
  void ClearOutputArr();
  /**
   * @brief 获取输出数组
   */
  const std::vector<IOutput::Ptr>& output_arr();
 private:
  /**
   * @brief 输出数组
   */
  std::vector<IOutput::Ptr> output_arr_;
  /**
   * @brief 数组操作读写锁
   */
  th::RWMutex rw_mutex_;
};

/**
 * @brief 日志类, 用于将事件输出至控制台和文件
 */
class Logger {
 public:
  using Ptr = std::shared_ptr<Logger>;
  /**
   * @brief 构建日志器并对格式进行解析
   * @param name 日志名字
   * @param level 日志输出等级
   * @param format_str 日志格式字符串
   * @throw 解析不成功抛出异常exception::ParseInvalidKey
   */
  Logger(std::string name, 
         Level::level level, 
         std::string format_str);
  /**
   * @brief 输出
   * @param e 日志事件
   */
  void Output(Event::Ptr event_ptr);
  /**
   * @brief 添加输出
   */
  void AddOutput(OutputMgr::IOutput::Ptr output) {
    output_mgr_->AddOutput(output);
  }
  /**
   * @brief 清空输出
   */
  void ClearOutputs() {
    output_mgr_->ClearOutputArr();
  }
  /**
   * @brief 获取日志名
   */
  std::string name() const {
    return name_;
  }
  /**
   * @brief 获取输出等级
   */
  Level::level level() const {
    return level_;
  }
  /**
   * @brief 获取格式管理器
   * @return FormattingMgr::Ptr 管理器指针
   */
  FormattingMgr::Ptr formatting_mgr() const {
    return formatting_mgr_;
  }
  /**
   * @brief 获取输出管理器
   * @return OutputMgr::Ptr 管理器指针
   */
  OutputMgr::Ptr output_mgr() const {
    return output_mgr_;
  }
  /**
   * @brief 设置输出等级
   * @param level 
   */
  void set_level(Level::level level) {
    level_ = level;
  }
  /**
   * @brief 设置格式管理器
   * @param formatting_mgr 管理器指针
   */
  void set_formatting_mgr(FormattingMgr::Ptr formatting_mgr);
  /**
   * @brief 设置输出管理器
   * @param output_mgr 管理器指针
   */
  void set_output_mgr(OutputMgr::Ptr output_mgr);
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
   * @brief 日志格式管理器
   */
  FormattingMgr::Ptr formatting_mgr_;
  /**
   * @brief 日志输出管理器
   */
  OutputMgr::Ptr output_mgr_;
  /**
   * @brief 日志器操作互斥锁
   */
  th::Mutex mutex_;
};

/**
 * @brief Logger输出对象(JSON)
 */
struct LoggerOutputerJsonObj {
  OUTPUT_TYPE type_;
  std::string path_;
};

/**
 * @brief Logger对象(JSON)
 */
struct LoggerJsonObj {
  std::string name_;
  std::string level_;
  std::string formatting_str_;
  std::vector<LoggerOutputerJsonObj> output_arr_;
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
   * @param key 日志器名
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
  cfg::Var<std::vector<LoggerJsonObj> > cfg_arr_;
  /**
   * @brief 日志器字典操作读写锁
   */
  th::RWMutex loggers_op_mutex_;
};

using Mgr = util::Single<Manager>;

/**
 * @brief 日志接口具体实现
 */
class Log::Impl {
 public:
  /**
   * @param event 事件指针
   * @param logger_name 日志器名
   */
  Impl(Event::Ptr event,
       std::string logger_name = DEFAULT_LOGGER_NAME);
  /**
   * @brief 日志接口实现层析构函数, 当析构触发时日志输出
   */
  ~Impl();
  /**
   * @brief 创建日志接口对象
   * @param event 事件指针
   * @param logger_name 日志名
   */
  static Log Create(Event::Ptr event,
                    std::string logger_name);
  /**
   * @brief 获取事件指针
   */
  Event::Ptr event() {
    return event_;
  }
 private:
  Event::Ptr event_;
  Logger::Ptr logger_;
};

} // namespace log
} // namespace seeker

#endif // _SEEKER_SRC_LOG_H_