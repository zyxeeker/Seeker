#include "thread.h"

#include <unistd.h>
#include <exception>

#include "log.h"

#define DEFAULT_THREAD_NAME    "UNKNOWN"
#define SYS_API_RES_CHECK(api) \
    api ? errno : 0

namespace seeker {
namespace th {

const char* MODULE_NAME = "seeker::th";

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

//// SpinMutex Begin
SpinMutex::Impl::Impl() {
  int res = pthread_spin_init(&mutex_, 0);
  if (res)
    throw RunTimeError::Create(MODULE_NAME, 
                               "pthread_spin_init fail to execute", 
                               res);
}

SpinMutex::Impl::~Impl() {
  if (!destoryed_) {
    try {
      Destory();
    } catch (seeker::RunTimeError ex) {
      log::Warn("system", MODULE_NAME, "~SpinMutex()", 0) << ex.what();
    }
  }
}

void SpinMutex::Impl::Destory() {
  int res = pthread_spin_destroy(&mutex_);
  if (res)
    throw RunTimeError::Create(MODULE_NAME, 
                               "pthread_spin_destroy fail to execute", 
                               res);
  destoryed_ = true; 
}

SpinMutex::SpinMutex()
    : impl_(std::make_unique<Impl>()) {}

SpinMutex::~SpinMutex() = default;

int SpinMutex::Lock() {
  return SYS_API_RES_CHECK(pthread_spin_lock(&(impl_->mutex_)));
}

int SpinMutex::Unlock() {
  return SYS_API_RES_CHECK(pthread_spin_unlock(&(impl_->mutex_)));
}
//// SpinMutex End

//// RWMutex Begin
RWMutex::Impl::Impl()
    : mutex_(PTHREAD_RWLOCK_INITIALIZER) {}

RWMutex::Impl::~Impl() {
  if (!destoryed_) {
    try {
      Destory();
    } catch (seeker::RunTimeError ex) {
      log::Warn("system", MODULE_NAME, "~RWMutex()", 0) << ex.what();
    }
  }
}

void RWMutex::Impl::Destory() {
  int res = pthread_rwlock_destroy(&mutex_);
  if (res)
    throw RunTimeError::Create(MODULE_NAME, 
                               "pthread_rwlock_destroy fail to execute", 
                               res);
  destoryed_ = true; 
}

RWMutex::RWMutex()
    : impl_(std::make_unique<Impl>()) {}

RWMutex::~RWMutex() = default;

int RWMutex::RDLock() {
  return SYS_API_RES_CHECK(pthread_rwlock_rdlock(&(impl_->mutex_)));
}

int RWMutex::WRLock() {
  return SYS_API_RES_CHECK(pthread_rwlock_wrlock(&(impl_->mutex_)));
}

int RWMutex::Unlock() {
  return SYS_API_RES_CHECK(pthread_rwlock_unlock(&(impl_->mutex_)));
}

void RWMutex::Destory() {
  impl_->Destory();
}
//// RWMutex End

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

TaskBase::Impl::Impl(std::string name, Func func)
    : name_(std::move(name)),
      func_(std::move(func)) {}

TaskBase::Impl::~Impl() = default;

TaskBase::TaskBase(std::string name, Func func)
    : impl_(std::make_unique<TaskBase::Impl>(std::move(name), std::move(func))) {}

TaskBase::~TaskBase() = default;

std::string TaskBase::name() const {
  return impl_->name();
}

time_t TaskBase::start_time() const {
  return impl_->start_time();
}

time_t TaskBase::done_time() const {
  return impl_->done_time();
}

time_t GetCurrentTimeMsec(){
	auto time = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
	time_t timestamp = time.time_since_epoch().count();
	return timestamp;
}

ThreadPool::Impl::Impl(size_t thread_num)
    : started_(false), 
      thread_num_(thread_num) {}

ThreadPool::Impl::~Impl() = default;

bool ThreadPool::Impl::Start() {
  std::lock_guard<std::mutex> l(mutex_);
  if (started_) {
    return true;
  }
  started_ = true;
  std::atomic<size_t> count{0};
  for (auto i = 0; i < thread_num_; i++)  {
    try {
      auto thread = std::thread([&](){
        count.fetch_add(1);
        Loop();
      });
      threads_.push_back(std::move(thread));
    } catch(const std::system_error& e) {
      std::cout << "Caught system_error with code "
                   "[" << e.code() << "] meaning "
                   "[" << e.what() << "]\n";
      --thread_num_;
    }
  }
  while (count.load() != thread_num_) {}
  return true;
}

void ThreadPool::Impl::Stop() {
  {
    std::lock_guard<std::mutex> l(mutex_);
    started_ = false;
    cv_.notify_all();
  }

  int index = 0;
  for (auto& thread : threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  std::lock_guard<std::mutex> l(mutex_);
  threads_.clear();
}

void ThreadPool::Impl::PushTask(TaskBase::Ptr&& task) {
  std::lock_guard<std::mutex> l(mutex_);
  tasks_.push(std::move(task));
  cv_.notify_one();
}

void ThreadPool::Impl::Loop() {
  while (started_) {
    std::unique_lock<std::mutex> cv_l(mutex_);
    cv_.wait(cv_l, [&](){ return !(tasks_.empty() && started_); });

    if (tasks_.empty() || !started_) {
      continue;
    }

    {
      auto& task = tasks_.front();
      if (!task) {
        continue;
      }
      
      task->impl_->set_start_time(GetCurrentTimeMsec());
      (task->impl_->func())();
      task->impl_->set_done_time(GetCurrentTimeMsec());
      
      tasks_.pop();
    }
  }
}

ThreadPool::ThreadPool(size_t thread_num)
    : impl_(std::make_unique<ThreadPool::Impl>(thread_num)) {}

ThreadPool::~ThreadPool() = default;

bool ThreadPool::Start() {
  return impl_->Start();
}

void ThreadPool::Stop() {
  impl_->Stop();
}

void ThreadPool::PushTask(TaskBase::Ptr task) {
  impl_->PushTask(std::move(task));
}

} // seeker