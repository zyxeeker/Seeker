/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-26 18:05:29
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-26 23:20:02
 * @Description: 
 */

#ifndef __SEEKER_TASK_HPP__
#define __SEEKER_TASK_HPP__

#include <future>
#include <memory>
#include <iostream>
#include <functional>

namespace seeker {

struct Task {
  std::function<void()> Func;
};

} // namespace seeker

#endif // __SEEKER_TASK_HPP__