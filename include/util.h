/**
 * @file util.h
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 工具辅助类
 * @version 1.0
 * @date 2023-06-03
 */

#ifndef __SEEKER_UTIL_H__
#define __SEEKER_UTIL_H__

#include <unistd.h>
#include <ctime>

namespace seeker {
/**
 * @brief 线程ID
 */
using TID = pid_t;
/**
 * @brief 进程ID
 */
using PID = pid_t;
} // seeker

namespace seeker {
namespace util {

/**
 * @brief 获取时间戳
 */
static uint64_t GetTimeStamp() {
  return time(nullptr);
}

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

/**
 * @brief 获取当前线程ID
 * @return TID 当前线程ID
 */
static TID GetThreadId() {
  return gettid();
}

} // util
} // seeker

#endif // __SEEKER_UTIL_H__