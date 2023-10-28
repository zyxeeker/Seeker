/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-27 10:30:27
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-28 15:06:26
 * @Description: 线程池实现
 */

#ifndef __SEEKER_THREAD_HPP__
#define __SEEKER_THREAD_HPP__

#include <future>
#include <memory>
#include <functional>

namespace seeker {

class ThreadPool {
  struct Task {
    using Ptr = std::shared_ptr<Task>;
    std::function<void()> Func;
  };
 public:
  ThreadPool(size_t thread_num);
  ~ThreadPool();

  bool Start();
  void Stop();
  
  template <class Func, typename ...Args>
  auto CreateTask(Func&& func, Args&&... args) {
    auto bind = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
    auto pkg_ptr = std::make_shared<std::packaged_task<decltype(func(args...))()> >(bind);
    
    auto task = std::make_shared<Task>();
    task->Func = [pkg_ptr](){
      (*pkg_ptr)();
    };
    auto res_native = pkg_ptr->get_future();
    PushTask(std::forward<Task::Ptr>(task));
    return res_native;
  }

  template <class Func, typename ...Args>
  auto CreateSharedTask(Func&& func, Args&&... args) {
    return std::shared_future<decltype(func(args...))>(
      CreateTask(std::forward<Func>(func), std::forward<Args>(args)...)
    );
  }

 protected:
  void PushTask(Task::Ptr&& task_ptr);
  
 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace seeker

#endif // __SEEKER_THREAD_HPP__
