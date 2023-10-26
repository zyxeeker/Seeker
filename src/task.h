/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-26 16:46:47
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-26 23:19:51
 * @Description: 
 */

#ifndef __SEEKER_SRC_TASK_HPP__
#define __SEEKER_SRC_TASK_HPP__

#include <queue>
#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <functional>
#include <condition_variable>

#include "../include/task.h"

namespace seeker {

using ThreadPtr = std::shared_ptr<std::thread>;
using TaskPtr = std::shared_ptr<Task>;

class TaskMgr {
 public:
  TaskMgr(size_t thread_num);
  ~TaskMgr();

  bool Start();
  void Stop();
  void AddTask(TaskPtr task);
  
 private:
  void Loop();

 private:
  bool started_;
  
  size_t thread_num_;
  std::vector<ThreadPtr> threads_;

  std::queue<TaskPtr> tasks_;
  
  std::mutex tasks_ops_mutex_;
  std::condition_variable cv_;
};

} // namespace seeker

#endif // __SEEKER_SRC_TASK_HPP__