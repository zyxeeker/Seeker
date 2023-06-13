/**
 * @file thread.h
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 线程模块的实现
 * @version 1.0
 * @date 2023-06-09
 */

#include <pthread.h>
#include <semaphore.h>
#include "../include/thread.h"

namespace seeker {
namespace th {

/**
 * @brief 线程中间实现
 */
struct Thread::Impl {
  Impl(std::function<void()> callback, 
       const std::string name);
  ~Impl();
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

  pthread_mutex_t mutex_;
};

/**
 * @brief 条件变量中间实现
 */
struct Cond::Impl {
  Impl();
  ~Impl();

  pthread_cond_t cond_;
};

/**
 * @brief 信号量中间实现
 */
struct Sem::Impl {
  Impl(uint32_t count);
  ~Impl();

  sem_t sem_;
};

} // th
} // seeker