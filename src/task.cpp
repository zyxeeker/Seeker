#include "task.h"

#include <atomic>
#include <iostream>

namespace seeker {

TaskMgr::TaskMgr(size_t thread_num = 10)
    : thread_num_(thread_num) {}

TaskMgr::~TaskMgr() = default;

bool TaskMgr::Start() {
  if (started_) {
    return true;
  }

  started_ = true;
  std::atomic<size_t> nums{0};

  for (auto i = 0; i < thread_num_; i++) {
    auto th = std::make_shared<std::thread>([&](){
      // std::cout << "THREAD NO." << nums.load() << " START!" << std::endl;
      nums.fetch_add(1);
      Loop();
    });
    threads_.push_back(std::move(th));
  }
  // make sure all thread start
  while (nums.load() != thread_num_) {}
  // std::cout << "NUMS: " << nums.load() << std::endl;
  return true;
}

void TaskMgr::Stop() {
  started_ = false;
  cv_.notify_all();

  int index = 0;
  for (auto& i : threads_) {
    i->join();
    // std::cout << "THREAD NO." << index++ << " EXIST!" << std::endl;
  }
}

void TaskMgr::AddTask(TaskPtr task) {
  std::lock_guard<std::mutex> l(tasks_ops_mutex_);
  tasks_.push(std::move(task));
  cv_.notify_one();
}

void TaskMgr::Loop() {
  while (started_) {
    std::mutex mutex;
    std::unique_lock<std::mutex> ul(mutex);
    cv_.wait(ul);
    
    if (tasks_.empty()) {
      continue;
    }
    std::lock_guard ll(tasks_ops_mutex_);
    auto task = tasks_.front();
    task->Func();
    tasks_.pop();
  }
}

} // namespace seeker