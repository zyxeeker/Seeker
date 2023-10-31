/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-05-30 15:18:04
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-31 12:01:27
 * @Description: 工具辅助类
 */

#ifndef _SEEKER_SRC_UTIL_HPP_
#define _SEEKER_SRC_UTIL_HPP_

#include <unistd.h>

#include <ctime>
#include <string>

#include "util.h"

namespace seeker {
namespace util {

#define DO_NOT_ASSIGN_AND_COPY(C) \
public:                           \
  C(C&&) = delete;                \
  C(const C&) = delete;           \
  C& operator=(const C&) = delete;

template <class T>
class Single {
 public:
  inline static T& GetInstance() {
    static T inst;
    return inst;
  }
  
  DO_NOT_ASSIGN_AND_COPY(Single)
};

/**
 * @brief 时间戳格式化
 * @param format 格式字符串
 * @param time_stamp 时间戳
 * @return std::string 格式化后的时间戳
 */
static std::string TimeStampToString(const std::string &format, uint64_t time_stamp) {
	time_t t = (time_t)time_stamp;
  time(&t);
  struct tm *info;
  info = localtime(&t);
  char buff[80];
  strftime(buff, 80, format.c_str(), info);
  return std::string(buff);
}

} // namespace util
} // namespace seeker

#endif //_SEEKER_SRC_UTIL_HPP_