/**
 * @file thread.h
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 线程模块的实现
 * @version 1.0
 * @date 2023-06-09
 */

#ifndef _SEEKER_SRC_THREAD_H_
#define _SEEKER_SRC_THREAD_H_

#include <queue>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <future>
#include <condition_variable>

#include <pthread.h>
#include <semaphore.h>
#include "../include/thread.h"
#include "../include/exception.h"
#include "../include/thread.hpp"

namespace seeker {
namespace th {
/**
 * @brief 线程中间实现
 */
struct Thread::Impl {
  Impl(std::function<void()> callback, 
       const std::string name);
  ~Impl();
  void Detach();
  
  /**
   * @brief 线程Id
   */
  TID id_;
  /**
   * @brief 线程名
   */
  std::string name_;
  /**
   * @brief 线程
   */
  pthread_t thread_;
  /**
   * @brief 线程执行的回调函数
   */
  std::function<void()> callback_;
  /**
   * @brief 判断是否已经进行过detach
   */
  bool detached_ = false;
 private:
  /**
  * @brief 线程运行函数
  */
  static void* Run(void* arg);
};

/**
 * @brief 互斥量中间实现
 */
struct Mutex::Impl {
  Impl();
  ~Impl();
  void Destory();

  /**
   * @brief 判断是否已进行销毁
   */
  bool destoryed_ = false;
  pthread_mutex_t mutex_;
};

/**
 * @brief 自旋锁中间实现
 */
struct SpinMutex::Impl {
  Impl();
  ~Impl();
  void Destory();

  /**
   * @brief 判断是否已进行销毁
   */
  bool destoryed_ = false;
  pthread_spinlock_t mutex_;
};

/**
 * @brief 读写锁中间实现
 */
struct RWMutex::Impl {
  Impl();
  ~Impl();
  void Destory();

  /**
   * @brief 判断是否已进行销毁
   */
  bool destoryed_ = false;
  pthread_rwlock_t mutex_;
};

/**
 * @brief 条件变量中间实现
 */
struct Cond::Impl {
  Impl();
  ~Impl();
  void Destory();

  /**
   * @brief 判断是否已进行销毁
   */
  bool destoryed_ = false;
  pthread_cond_t cond_;
};

/**
 * @brief 信号量中间实现
 */
struct Sem::Impl {
  Impl(uint32_t count);
  ~Impl();
  void Destory();

  /**
   * @brief 判断是否已进行销毁
   */
  bool destoryed_ = false;
  sem_t sem_;
};

} // th

class ThreadPool::Impl {
 public:
  Impl(size_t thread_num);
  ~Impl();

  bool Start();
  void Stop();
  void PushTask(Task::Ptr&& task);
  
 private:
  void Loop();

 private:
  bool started_;
  size_t thread_num_;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::queue<Task::Ptr> tasks_;
  std::vector<std::thread> threads_;  
};

} // namespace seeker

#endif // _SEEKER_SRC_THREAD_H_