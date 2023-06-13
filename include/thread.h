/**
 * @file thread.h
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 线程模块
 * @version 1.0
 * @date 2023-06-09
 */

#include <memory>
#include <string>
#include <functional>
#include "util.h"

namespace seeker {
namespace th {

/**
 * @brief 线程
 */
class Thread {
 public:
  using Ptr = std::shared_ptr<Thread>;
  Thread(std::function<void()> callback = nullptr, 
         const std::string name = "");
  ~Thread();
  void Join();
  /**
   * @brief 返回线程Id
   */
  TID id() const;
  /**
   * @brief 返回完整线程名
   */
  std::string name() const;
  /**
   * @brief 返回传入的函数指针
   */
  std::function<void()> callback() const;
 public:
  struct Impl;
 private:
  std::shared_ptr<Impl> impl_;

  DO_NOT_ASSIGN_AND_COPY(Thread);
};

/**
 * @brief 获取当前线程名
 */
std::string GetThreadName();

/**
 * @brief 获取当前线程Id
 */
TID GetThreadId();

/**
 * @brief 互斥量(以默认值创建)
 */
class Mutex {
 public:
  Mutex();
  ~Mutex();
  int Lock();
  int Unlock();
 private:
  struct Impl;
  std::shared_ptr<Impl> impl_;
  friend class Cond;
};

/**
 * @brief 条件变量(以默认值创建)
 */
class Cond {
 public:
  Cond();
  ~Cond();
  int Wait(const Mutex& mutex);
  int Signal();
  int BroadCast();
 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

/**
 * @brief 信号量
 */
class Sem {
 public:
  Sem(uint32_t count = 0);
  ~Sem();
  int Wait();
  int Post();
 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

/**
 * @brief SG
 */
class ScopeGuard {
 public:
  ScopeGuard(Mutex& mutex)
    : mutex_(mutex) {
      mutex_.Lock();
    }
  ~ScopeGuard() {
    mutex_.Unlock();
  }
 private:
  Mutex& mutex_;
};

} // th
} // seeker