/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-31 16:51:22
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-07 22:26:56
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
    virtual void Output(const std::ostringstream& oss) = 0;
  };

 public:
  Outputer();
  Outputer(std::vector<LoggerOutputDefineMeta> meta);

  /**
   * @brief 添加输出
   */
  inline void AddItem(IItem::Ptr output) {
    items_.push_back(output);
  }
  /**
   * @brief 清空输出数组
   */
  inline void ClearItems() {
    items_.clear();
  }
  /**
   * @brief 获取输出数组
   */
  const std::vector<IItem::Ptr>& items() const {
    return items_;
  }

 private:
  /**
   * @brief 输出数组
   */
  std::vector<IItem::Ptr> items_;
};

} // namespace log
} // namespace seeker

#endif // __SEEKER_SRC_LOG_OUTPUTER_H__