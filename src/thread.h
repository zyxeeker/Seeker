/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-06-09 17:12:31
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-30 15:14:43
 * @Description: 线程池的实现
 */

#ifndef __SEEKER_SRC_THREAD_H__
#define __SEEKER_SRC_THREAD_H__

#include <queue>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <future>
#include <condition_variable>

#include <pthread.h>
#include <semaphore.h>
#include "../include/exception.h"
#include "../include/thread.hpp"

namespace seeker {
class TaskBase::Impl {
 public:
  Impl(std::string name, Func func);
  ~Impl();

  inline std::string name() const {
    return name_;
  }
  inline time_t start_time() const {
    return start_time_;
  }
  inline void set_start_time(time_t time) {
    start_time_ = time;
  }
  inline time_t done_time() const {
    return done_time_;
  }
  inline void set_done_time(time_t time) {
    done_time_ = time;
  }
  inline const Func& func() const {
    return func_;
  }
 private:
  std::string name_;
  time_t start_time_ = 0;
  time_t done_time_ = 0;
  Func func_;
};

class ThreadPool::Impl {
 public:
  Impl(size_t thread_num);
  ~Impl();

  bool Start();
  void Stop();
  void PushTask(TaskBase::Ptr&& task);
  
 private:
  void Loop();

 private:
  bool started_;
  size_t thread_num_;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::queue<TaskBase::Ptr> tasks_;
  std::vector<std::thread> threads_;  
};

} // namespace seeker

#endif // _SEEKER_SRC_THREAD_H__