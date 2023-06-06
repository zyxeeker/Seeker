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
#include "../include/util.h"
#include "util.h"

namespace seeker {
namespace log {

#define DEFAULT_LOGGER_NAME           "root"
#define DEFAULT_FORMATTER_PATTERN     "%d [%P](%r){%F:%L(%N)} %m"
#define DEFAULT_DATETIME_PATTERN      "%Y-%m-%d %H:%M"
#define EMPTY_PARAM                   ""

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
    virtual void ToStream(std::stringstream &ss, 
                          std::string& logger_name, 
                          Event::Ptr e) = 0;
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
    virtual void Output(const std::string &buf) = 0;
  };
  /**
   * @brief 添加输出
   */
  void AddOutput(IOutput::Ptr output) {
    output_arr_.push_back(output);
  }
  /**
   * @brief 清空输出数组
   */
  void ClearOutputArr() {
    output_arr_.clear();
  }
  /**
   * @brief 获取输出数组
   */
  const std::vector<IOutput::Ptr>& output_arr() const {
    return output_arr_;
  }
 private:
  /**
   * @brief 输出数组
   */
  std::vector<IOutput::Ptr> output_arr_;
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
   * @param format_str 日志格式字符串
   * @throw 解析不成功抛出异常exception::ParseInvalidKey
   */
  Logger(std::string name, std::string format_str);
  /**
   * @brief 输出
   * @param e 日志事件
   */
  void Output(Event::Ptr e);
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
   * @brief 设置格式管理器
   * @param formatting_mgr 管理器指针
   */
  void set_formatting_mgr(FormattingMgr::Ptr formatting_mgr) {
    formatting_mgr_ = formatting_mgr;
  }
  /**
   * @brief 设置输出管理器
   * @param output_mgr 管理器指针
   */
  void set_output_mgr(OutputMgr::Ptr output_mgr) {
    output_mgr_ = output_mgr;
  }
 private:
  /**
    * @brief 日志名字
    */
  std::string name_;
  /**
   * @brief 日志格式管理器
   */
  FormattingMgr::Ptr formatting_mgr_;
  /**
   * @brief 日志输出管理器
   */
  OutputMgr::Ptr output_mgr_;
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
 private:
  /**
  * @brief 日志器字典
  */
  std::unordered_map<std::string, Logger::Ptr> loggers_;
  /**
   * @brief 默认日志器
   */
  Logger::Ptr default_logger_;
};

using Mgr = Single<Manager>;

/**
 * @brief 日志接口实现层
 */
class Log::Impl {
 public:
  /**
   * @param level 日志等级
   * @param file_name 文件名
   * @param function_name 函数名
   * @param line_num 行号
   * @param timestamp 时间戳
   * @param thread_id 线程id
   * @param logger_name 线程名
   */
  Impl(Level::level level,
       const char* file_name, 
       const char* function_name, 
       int line_num,
       uint64_t timestamp,
       TID thread_id,
       std::string logger_name = DEFAULT_LOGGER_NAME);       
  /**
   * @brief 日志接口实现层析构函数, 当析构触发时输出日志
   */
  ~Impl();
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