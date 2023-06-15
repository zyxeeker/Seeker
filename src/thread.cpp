/**
 * @file thread.cpp
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 线程模块的实现
 * @version 1.0
 * @date 2023-06-09
 */

#include "thread.h"
#include <unistd.h>
#include "log.h"

#define DEFAULT_THREAD_NAME    "UNKNOWN"
#define SYS_API_RES_CHECK(api) \
    api ? errno : 0

namespace seeker {
namespace th {

//// Thread Begin
/**
 * @brief 用于在线程的栈区存储Thread::Impl指针
 */
static thread_local Thread::Impl* kThreadImpl = nullptr;

Thread::Impl::Impl(std::function<void()> callback, 
                   const std::string name)
    : callback_(callback),
      name_(name.length() ? name : DEFAULT_THREAD_NAME) {
  int res = pthread_create(&thread_, nullptr, Impl::Run, this);
  if (res)
    throw RunTimeError::Create(MODULE_NAME, 
                               "\"" + name_ + "\" thread fail to created", 
                               res);
}

Thread::Impl::~Impl() {
  if (!detached_) {
    try {
      Detach();
    } catch (seeker::RunTimeError ex) {
      log::Warn("system", MODULE_NAME, "~Thread()", 0) << ex.what();
    }
  }
}

void Thread::Impl::Detach() {
  if (thread_) {
    int res = pthread_detach(thread_);
    if (res)
      throw RunTimeError::Create(MODULE_NAME, 
                                 "\"" + name_ + "\" thread fail to detach", 
                                 res);
    detached_ = true;
  }
}

void* Thread::Impl::Run(void* arg) {
  kThreadImpl = (Impl*)arg;
  // 获取线程id
  kThreadImpl->id_ = GetThreadId();
  // 设置线程名, pthread_setname_np 限制的名字长度为16
  pthread_setname_np(kThreadImpl->thread_, 
                     kThreadImpl->name_.substr(0, 15).c_str());
  auto cb = kThreadImpl->callback_;
  // 判断callback是否为空
  if (cb)
    cb();
  return 0;
}

Thread::Thread(std::function<void()> callback, 
               const std::string name)
    : impl_(std::make_unique<Impl>(callback, name)) {}

Thread::~Thread() = default;

void Thread::Join() {
  if (impl_->thread_) {
    int res = pthread_join(impl_->thread_, nullptr);
    if (res)
      throw RunTimeError::Create(MODULE_NAME, 
                                 "\"" + impl_->name_ + "\" thread fail to join", 
                                 res);
    impl_->thread_ = 0;
  }
}

void Thread::Detach() {
  impl_->Detach();
}

TID Thread::id() const {
  return impl_->id_;
}

std::string Thread::name() const {
  return impl_->name_;
}

std::function<void()> Thread::callback() const {
  return impl_->callback_;
}

std::string GetThreadName() {
  return kThreadImpl ? kThreadImpl->name_ : DEFAULT_THREAD_NAME;
}

TID GetThreadId() {
  return kThreadImpl ? kThreadImpl->id_ : gettid();
}
//// Thread End

//// Mutex Begin
Mutex::Impl::Impl()
    : mutex_(PTHREAD_MUTEX_INITIALIZER) {}

Mutex::Impl::~Impl() {
  if (!destoryed_) {
    try {
      Destory();
    } catch (seeker::RunTimeError ex) {
      log::Warn("system", MODULE_NAME, "~Mutex()", 0) << ex.what();
    }
  }
}

void Mutex::Impl::Destory() {
  int res = pthread_mutex_destroy(&mutex_);
  if (res)
    throw RunTimeError::Create(MODULE_NAME, 
                               "pthread_mutex_destroy fail to execute", 
                               res);
  destoryed_ = true; 
}

Mutex::Mutex()
    : impl_(std::make_unique<Impl>()) {}

Mutex::~Mutex() = default;

int Mutex::Lock() {
  return SYS_API_RES_CHECK(pthread_mutex_lock(&(impl_->mutex_)));
}

int Mutex::Unlock() {
  return SYS_API_RES_CHECK(pthread_mutex_unlock(&(impl_->mutex_)));
}
//// Mutex End

//// Condition Begin
Cond::Impl::Impl()
    : cond_(PTHREAD_COND_INITIALIZER) {}

Cond::Impl::~Impl() {
  if (!destoryed_) {
    try {
      Destory();
    } catch (seeker::RunTimeError ex) {
      log::Warn("system", MODULE_NAME, "~Cond()", 0) << ex.what();
    }
  }
}

void Cond::Impl::Destory() {
  int res = pthread_cond_destroy(&cond_);
  if (res)
    throw RunTimeError::Create(MODULE_NAME, 
                               "pthread_cond_destroy fail to execute", 
                               res);
  destoryed_ = true;
}

Cond::Cond() 
    : impl_(std::make_unique<Impl>()) {}

Cond::~Cond() = default;

int Cond::Wait(const Mutex& mutex) {
  return SYS_API_RES_CHECK(
             pthread_cond_wait(&(impl_->cond_), &(mutex.impl_->mutex_)));
}

int Cond::Signal() {
  return SYS_API_RES_CHECK(pthread_cond_signal(&(impl_->cond_)));
}

int Cond::BroadCast() {
  return SYS_API_RES_CHECK(pthread_cond_broadcast(&(impl_->cond_)));
}

void Cond::Destory() {
  impl_->Destory();
}
//// Condition End

//// Semaphore Begin
Sem::Impl::Impl(uint32_t count) {
  if (sem_init(&sem_, 0, count))
    throw RunTimeError::Create(MODULE_NAME, 
                               "sem_init fail to execute", 
                               errno);
}

Sem::Impl::~Impl() {
  if (!destoryed_) {
    try {
      Destory();
    } catch(seeker::RunTimeError ex) {
      log::Warn("system", MODULE_NAME, "~Sem()", 0) << ex.what();
    }
  }
}

void Sem::Impl::Destory() {
  if (sem_destroy(&sem_))
    throw RunTimeError::Create(MODULE_NAME, 
                               "sem_destroy fail to execute", 
                               errno);
  destoryed_ = true;
}

Sem::Sem(uint32_t count)
    : impl_(std::make_unique<Sem::Impl>(count)) {}

Sem::~Sem() = default;

int Sem::Wait() {
  return SYS_API_RES_CHECK(sem_wait(&(impl_->sem_)));
}

int Sem::Post() {
  return SYS_API_RES_CHECK(sem_post(&(impl_->sem_)));
}

void Sem::Destory() {
  impl_->Destory();
}

//// Semaphore End
} // th
} // seeker