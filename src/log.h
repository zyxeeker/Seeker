/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-31 17:19:37
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-31 20:51:02
 * @Description: 
 */

#include "../include/log.h"

#include "logger/core.h"

namespace seeker {
namespace log {

/**
 * @brief 日志接口具体实现
 */
class Log::Impl {
 public:
  /**
   * @param event 事件指针
   */
  Impl(Event::Ptr event,
       std::string logger_name = DEFAULT_LOGGER_NAME);
  /**
   * @brief 日志接口实现层析构函数, 当析构触发时日志输出
   */
  ~Impl();
  /**
   * @brief 创建日志接口对象
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