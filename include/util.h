/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-06-03 16:44:19
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-31 14:28:01
 * @Description: 工具辅助类
 */

#ifndef __SEEKER_UTIL_H__
#define __SEEKER_UTIL_H__

#include <unistd.h>
#include <chrono>

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
inline static time_t GetCurTimeStamp() {
	auto time = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
	time_t timestamp = time.time_since_epoch().count();
	return timestamp;
}

} // util
} // seeker

#endif // __SEEKER_UTIL_H__