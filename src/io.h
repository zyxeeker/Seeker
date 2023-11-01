/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-11-01 11:21:10
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-01 15:15:52
 * @Description: 
 */

#ifndef __SEEKER_SRC_IO_H__
#define __SEEKER_SRC_IO_H__

#include <unordered_map>

#include "util.hpp"

#include "../include/thread.hpp"

namespace seeker {
namespace io {

class Manager {
 public:
  
  class Service : public seeker::ThreadPool {
   public:
    using Ptr = std::shared_ptr<Service>;
    using WPtr = std::weak_ptr<Service>;

    Service(size_t thread_num)
        : seeker::ThreadPool(thread_num) {}
    ~Service() = default;
  };

 public:
  enum TYPE {
    TINY_FILE_SERVICE,
    // TODO: Support More...
  };

 public:
  Manager();

  bool GetService(TYPE type, Service::WPtr& wptr);
  
 private:
  void InitService();

 private:
  std::mutex mutex_;
  std::unordered_map<TYPE, Service::Ptr> service_;
};

using Mgr = util::Single<Manager>;

} // namespace io
} // namespace seeker

#endif // __SEEKER_SRC_IO_H__