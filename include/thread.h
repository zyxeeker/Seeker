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
  /**
   * @brief 构造时创建线程
   * @throw seeker::RunTimeError
   */
  Thread(std::function<void()> callback = nullptr, 
         const std::string name = "");
  /**
   * @brief 析构时触发Detach, 调用Detach()函数后析构时不再触发
   */
  ~Thread();
  /**
   * @throw seeker::RunTimeError
   */
  void Join();
  /**
   * @throw seeker::RunTimeError
   */
  void Detach();
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
extern std::string GetThreadName();

/**
 * @brief 获取当前线程Id
 */
extern TID GetThreadId();

/**
 * @brief 互斥量(以默认值创建)
 */
class Mutex {
 public:
  /**
   * @brief 构造时进行初始化
   * @throw seeker::RunTimeError
   */
  Mutex();
  /**
   * @brief 析构时触发销毁, 调用Destory()后析构不再触发销毁
   */
  ~Mutex();
  /**
   * @return int 成功返回0, 失败则返回错误值
   */
  int Lock();
  /**
   * @return int 成功返回0, 失败则返回错误值
   */
  int Unlock();
  /**
   * @brief 销毁
   * @throw seeker::RunTimeError
   */
  void Destory();
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
  /**
   * @brief 构造时自动创建
   * @throw seeker::RunTimeError
   */
  Cond();
  /**
   * @brief 析构时触发销毁, 调用Destory()后析构不再触发销毁
   */
  ~Cond();
  /**
   * @return int 成功返回0, 失败则返回错误值
   */
  int Wait(const Mutex& mutex);
  /**
   * @return int 成功返回0, 失败则返回错误值
   */
  int Signal();
  /**
   * @return int 成功返回0, 失败则返回错误值
   */
  int BroadCast();
  /**
   * @brief 销毁
   * @throw seeker::RunTimeError
   */
  void Destory();
 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

/**
 * @brief 信号量
 */
class Sem {
 public:
  /**
   * @brief 构造时进行初始化
   * @throw seeker::RunTimeError
   */
  Sem(uint32_t count = 0);
  /**
   * @brief 析构时触发销毁, 调用Destory()后析构不会触发销毁
   */
  ~Sem();
  /**
   * @return int 成功返回0, 失败则返回错误值
   */
  int Wait();
  /**
   * @return int 成功返回0, 失败则返回错误值
   */
  int Post();
  /**
   * @brief 销毁
   * @throw seeker::RunTimeError
   */
  void Destory();
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