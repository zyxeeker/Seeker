#include "thread.h"

#include <unistd.h>

#include "log.h"
#include "util.h"

#define DEFAULT_THREAD_NAME    "UNKNOWN"

namespace seeker {

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

ThreadPool::Impl::Impl(size_t thread_num)
    : started_(false), 
      thread_num_(thread_num) {}

ThreadPool::Impl::~Impl() {
  Stop();
}

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
      
      task->impl_->set_start_time(util::GetCurTimeStamp());
      (task->impl_->func())();
      task->impl_->set_done_time(util::GetCurTimeStamp());
      
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

} // namespace seeker