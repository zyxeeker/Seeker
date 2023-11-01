/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-25 10:02:24
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-01 11:29:52
 * @Description: 网络接口具体实现
 */

#ifndef __SEEKER_SRC_NET_H__
#define __SEEKER_SRC_NET_H__

#include <mutex>
#include <memory>
#include <unordered_map>

#include "net.hpp"

#include "util.hpp"

namespace seeker {
namespace net {

class Manager {
 public:
  enum TYPE {
    SERVICE_HTTP,
    // TODO:Support More...
  };

  struct ServiceMeta {
    INetService::Ptr Ptr;
    TYPE Type;
  };

 public:
  bool RegisterService(const std::string& name, TYPE type, INetService::Ptr ptr);
  void UnregisterService(const std::string& name);

  inline const std::unordered_map<std::string, ServiceMeta>& service() const {
    return service_;
  }
 private:
  std::mutex mutex_;
  std::unordered_map<std::string, ServiceMeta> service_;
};

using Mgr = util::Single<Manager>;

} // namespace net
} // namespace seeker

#endif // __SEEKER_SRC_NET_H__