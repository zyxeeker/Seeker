/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-27 10:30:27
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-30 15:02:19
 * @Description: 线程池实现
 */

#ifndef __SEEKER_THREAD_HPP__
#define __SEEKER_THREAD_HPP__

#include <string>
#include <future>
#include <memory>
#include <functional>
#include <chrono>

namespace seeker {

class TaskBase {
 public:
  using Func = std::function<void()>;
  using Ptr = std::shared_ptr<TaskBase>;

  TaskBase(std::string name, Func func);
  ~TaskBase();

  std::string name() const;
  time_t start_time() const;
  time_t done_time() const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;

  friend class ThreadPool;
};

template <typename T>
class Task : public TaskBase {
 public:
  Task(std::string name, Func func)
      : TaskBase(std::move(name), std::move(func)) {}

  std::future<T>& result() {
    return result_;
  }
 protected:
  void set_result(std::future<T> result) {
    result_ = std::move(result);
  }
  friend class ThreadPool;
 private:
  std::future<T> result_;
};

template <typename T>
class SharedTask : public TaskBase {
 public:
  SharedTask(std::string name, Func func)
      : TaskBase(std::move(name), std::move(func)) {}

  std::shared_future<T>& result() {
    return result_;
  }
 protected:
  void set_result(std::future<T>&& result) {
    result_ = result;
  }
  friend class ThreadPool;
 private:
  std::shared_future<T> result_;
};

class ThreadPool {
 public:
  ThreadPool(size_t thread_num);
  ~ThreadPool();

  bool Start();
  void Stop();

  template <class Func, typename ...Args>
  auto CreateTask(std::string name, Func&& func, Args&&... args) -> std::shared_ptr<Task<decltype(func(args...))> > {
    return CreateTaskPkg<Task<decltype(func(args...))> >(name,
                                                         std::forward<Func>(func), 
                                                         std::forward<Args>(args)...);
  }

  template <class Func, typename ...Args>
  auto CreateSharedTask(std::string name, Func&& func, Args&&... args) -> std::shared_ptr<SharedTask<decltype(func(args...))> > {
    return CreateTaskPkg<SharedTask<decltype(func(args...))> >(name,
                                                               std::forward<Func>(func), 
                                                               std::forward<Args>(args)...);
  }

 protected:
 template <typename U, class Func, typename ...Args>
  auto CreateTaskPkg(std::string name, Func&& func, Args&&... args) {
    auto bind = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
    auto pkg_ptr = std::make_shared<std::packaged_task<decltype(func(args...))()> >(bind);
    auto task = std::make_shared<U>(std::move(name), [pkg_ptr](){
      (*pkg_ptr)();
    });
    task->set_result(pkg_ptr->get_future());
    PushTask(std::dynamic_pointer_cast<TaskBase>(task));
    return task;
  }

  void PushTask(TaskBase::Ptr task_ptr);
  
 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

} // namespace seeker

#endif // __SEEKER_THREAD_HPP__
