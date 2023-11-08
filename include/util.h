/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-06-03 16:44:19
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-01 17:17:03
 * @Description: 工具辅助类
 */

#ifndef __SEEKER_UTIL_H__
#define __SEEKER_UTIL_H__

#include <unistd.h>
#include <chrono>

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

} // namespace util
} // namespace seeker

#endif // __SEEKER_UTIL_H__