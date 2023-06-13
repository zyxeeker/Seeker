/**
 * @file thread.cpp
 * @author zyxeeker (zyxeeker@gmail.com)
 * @brief 线程模块的实现
 * @version 1.0
 * @date 2023-06-09
 */

#include "thread.h"
#include <unistd.h>

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
  if (pthread_create(&thread_, nullptr, Impl::Run, this)) {
    // TODO: throw exception
  }
}

Thread::Impl::~Impl() {
  if (thread_) {
    if (pthread_detach(thread_)) {
      // TODO: throw exception
    }
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
    : impl_(std::make_shared<Impl>(callback, name)) {}

Thread::~Thread() = default;

void Thread::Join() {
  if (impl_->thread_) {
    pthread_join(impl_->thread_, nullptr);
    impl_->thread_ = 0;
  }
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
  if (pthread_mutex_destroy(&mutex_)) {
    // TODO: throw exception
  }
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
  if (!pthread_cond_destroy(&cond_)) {
    // TODO: throw exception
  }
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
//// Condition End
//// Semaphore Begin
Sem::Impl::Impl(uint32_t count) {
  if (sem_init(&sem_, 0, count)) {
    // TODO: throw exception
  }
}

Sem::Impl::~Impl() {
  if (sem_destroy(&sem_)) {
    // TODO: throw exception
  }
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
//// Semaphore Begin
} // th
} // seeker