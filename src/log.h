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
 * @brief 日志事件
 */
struct Event {
  using Ptr = std::shared_ptr<Event>;
  /**
   * @brief 等级
   */
  Level::level level;
  /**
   * @brief 文件名
   */
  const char* file_name;
  /**
   * @brief 函数名
   */
  const char* function;
  /**
   * @brief 行号
   */
  uint32_t line;
  /**
   * @brief 时间戳
   */
  uint64_t time;
  /**
   * @brief 线程ID
   */
  TID thread_id;
  /**
   * @brief 线程名
   */
  std::string thread_name;
  /**
   * @brief 内容
   */
  std::string content;
};

/**
 * @brief 日志格式管理类, 用于解析格式和存储解析后各子项的顺序
 */
class FormattingMgr {
 public:
  using Ptr = std::shared_ptr<FormattingMgr>;
  FormattingMgr(std::string format_str) 
    : m_raw(std::move(format_str)) {}
  /**
   * @brief 初始化对格式进行解析
   * @throw 不成功则抛出异常exception::ParseInvalidKey
   */
  void Init();
  /**
   * @brief 返回格式字符串
   * @return std::string 
   */
  std::string GetRaw() const {
    return m_raw;
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
    m_item_arr.push_back(item);
  }
  /**
   * @brief 清空子项数组
   */
  void ClearItemArr() {
    m_item_arr.clear();
  }
  /**
   * @brief 返回子项数组
   * @return const std::vector<IItem::Ptr>& 
   */
  const std::vector<IItem::Ptr>& GetItemArr() const {
    return m_item_arr;
  }
 private:
  /**
   * @brief 原始格式字符串
   */
  std::string m_raw;
  /**
   * @brief 解析后的格式子项数组
   */
  std::vector<IItem::Ptr> m_item_arr;
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
    m_output_arr.push_back(output);
  }
  /**
   * @brief 清空输出数组
   */
  void ClearOutputArr() {
    m_output_arr.clear();
  }
  /**
   * @brief 获取输出数组
   */
  const std::vector<IOutput::Ptr>& GetOutputArr() const {
    return m_output_arr;
  }
 private:
  /**
   * @brief 输出数组
   */
  std::vector<IOutput::Ptr> m_output_arr;
};

/**
 * @brief 日志类, 用于将事件输出至控制台和文件
 */
class Obj {
 public:
  using Ptr = std::shared_ptr<Obj>;
  /**
   * @brief 构建日志器并对格式进行解析
   * @param name 日志名字
   * @param format_str 日志格式字符串
   * @throw 解析不成功抛出异常exception::ParseInvalidKey
   */
  Obj(std::string name, std::string format_str);
  /**
   * @brief 输出
   * @param e 日志事件
   */
  void Output(Event::Ptr e);
  /**
   * @brief 添加输出
   */
  void AddOutput(OutputMgr::IOutput::Ptr output) {
    m_output_mgr->AddOutput(output);
  }
  /**
   * @brief 清空输出
   */
  void ClearOutputs() {
    m_output_mgr->ClearOutputArr();
  }
  /**
   * @brief 获取日志名
   */
  std::string GetName() const {
    return m_name;
  }
  /**
   * @brief 获取格式管理器
   * @return FormattingMgr::Ptr 管理器指针
   */
  FormattingMgr::Ptr GetFormattingMgr() const {
    return m_formatting_mgr;
  }
  /**
   * @brief 获取输出管理器
   * @return OutputMgr::Ptr 管理器指针
   */
  OutputMgr::Ptr GetOutputMgr() const {
    return m_output_mgr;
  }
  /**
   * @brief 设置格式管理器
   * @param formatting_mgr 管理器指针
   */
  void SetFormattingMgr(FormattingMgr::Ptr formatting_mgr) {
    m_formatting_mgr = formatting_mgr;
  }
  /**
   * @brief 设置输出管理器
   * @param output_mgr 管理器指针
   */
  void SetOutputMgr(OutputMgr::Ptr output_mgr) {
    m_output_mgr = output_mgr;
  }
 private:
  /**
    * @brief 日志名字
    */
  std::string m_name;
  /**
   * @brief 日志格式管理器
   */
  FormattingMgr::Ptr m_formatting_mgr;
  /**
   * @brief 日志输出管理器
   */
  OutputMgr::Ptr m_output_mgr;
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
  Obj::Ptr GetLogger(std::string key);
  /**
   * @brief 返回默认日志器 
   */
  Obj::Ptr GetDefaultLogger() {
    return m_default_logger;
  }
  /**
   * @brief 添加日志器
   */
  void AddLogger(Obj::Ptr l);
  /**
   * @brief 删除指定日志器
   */
  void DeleteLogger(std::string logger_name);
 private:
  /**
  * @brief 日志器字典
  */
  std::unordered_map<std::string, Obj::Ptr> m_loggers;
  /**
   * @brief 默认日志器
   */
  Obj::Ptr m_default_logger;
};

using Mgr = Single<Manager>;

} // namespace log
} // namespace seeker

#endif // _SEEKER_SRC_LOG_H_