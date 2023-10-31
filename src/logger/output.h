/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-31 16:51:22
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-31 20:52:02
 * @Description: 
 */

#ifndef __SEEKER_SRC_LOG_OUTPUTER_H__
#define __SEEKER_SRC_LOG_OUTPUTER_H__

#include "../include/cfg.hpp"

#include "formatter.h"

namespace seeker {
namespace log {

class Outputer {
 public:
  using Ptr = std::shared_ptr<Outputer>;

  struct Meta {
    std::string Type;
    std::string Path;

    DEFINE_PROPERTIES(
      Meta,
      PROPERTY_SCHME(Type, "type"),
      PROPERTY_SCHME(Path, "path")
    )
  };

  /**
   * @brief 日志输出接口(子模块)
   */
  class IItem {
   public:
    using Ptr = std::shared_ptr<IItem>;
    virtual ~IItem() = default;
    /**
     * @brief 输出接口
     */
    virtual void Output(const std::shared_ptr<Logger> logger,
                        const std::vector<Formatter::IItem::Ptr>& items,
                        const std::shared_ptr<Event> event_ptr) = 0;
  };

 public:
  Outputer();
  Outputer(std::vector<Meta> meta);
  /**
   * @brief 添加输出
   */
  void AddItem(IItem::Ptr output) {
    std::lock_guard<std::mutex> l(mutex_);
    items_.push_back(output);
  }
  /**
   * @brief 清空输出数组
   */
  void ClearItems() {
    std::lock_guard<std::mutex> l(mutex_);
    items_.clear();
  }
  /**
   * @brief 获取输出数组
   */
  const std::vector<IItem::Ptr>& items() {
    return items_;
  }

 private:
  /**
   * @brief 输出数组
   */
  std::vector<IItem::Ptr> items_;

  std::mutex mutex_;
};

} // namespace log
} // namespace seeker

#endif // __SEEKER_SRC_LOG_OUTPUTER_H__