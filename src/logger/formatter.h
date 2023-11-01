/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-31 16:17:54
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-01 11:17:25
 * @Description: 
 */

#ifndef __SEEKER_SRC_LOG_FORMATTER_H__
#define __SEEKER_SRC_LOG_FORMATTER_H__

#include <mutex>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../include/log.h"

namespace seeker {
namespace log {

namespace level {

static std::string ToString(log::LEVEL l) {
    switch (l) {
#define TRANS(name) \
  case log::LEVEL::name:\
      return #name;

  TRANS(DEBUG)
  TRANS(INFO)
  TRANS(WARN)
  TRANS(ERROR)
  TRANS(FATAL)
#undef TRANS
    default:
      return "UNKNOWN";
  }
}

static log::LEVEL FromString(std::string l_str) {
#define TRANS(level) \
  if (l_str == #level) \
    return log::LEVEL::level;

  TRANS(DEBUG)
  TRANS(INFO)
  TRANS(WARN)
  TRANS(ERROR)
  TRANS(FATAL)
#undef TRANS

  return log::LEVEL::UNKNOWN;
}

} // namespace level

class Logger;
class Event;

/**
 * @brief 日志格式管理类, 用于解析格式和存储解析后各子项的顺序
 */
class Formatter {
 public:
  using Ptr = std::shared_ptr<Formatter>;
  /**
   * @brief 格式参数项接口(子模块)
   */
  class IItem {
   public:
    using Ptr = std::shared_ptr<IItem>;
    virtual ~IItem() = default;
    /**
     * @brief 将数据进行转换并输出到传入的流中
     */
    virtual void ToStream(std::ostream &os, 
                          const std::shared_ptr<Logger> logger, 
                          const std::shared_ptr<Event> event_ptr) = 0;
  };

 public:
  Formatter(std::string format_str) 
      : raw_(std::move(format_str)) {}
  /**
   * @brief 初始化对格式进行解析
   * @throw 不成功则抛出异常exception::ParseInvalidKey
   */
  void Init();
  /**
   * @brief 添加子项
   */
  inline void AddItem(IItem::Ptr item) {
    items_.push_back(item);
  }
  /**
   * @brief 清空子项数组
   */
  inline void ClearItems() {
    items_.clear();
  }
  /**
   * @brief 返回格式字符串 
   */
  const std::string& raw() const {
    return raw_;
  }
  /**
   * @brief 返回子项数组
   */
  const std::vector<IItem::Ptr>& items() const {
    return items_;
  }

 private:
  /**
   * @brief 原始格式字符串
   */
  std::string raw_;
  /**
   * @brief 解析后的格式子项数组
   */
  std::vector<IItem::Ptr> items_;
};

} // namespace log
} // namespace seeker

#endif // __SEEKER_SRC_LOG_FORMATTER_H__
