/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-25 10:02:24
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-25 18:51:03
 * @Description: 网络接口具体实现
 */

#ifndef __SEEKER_SRC_NET_H__
#define __SEEKER_SRC_NET_H__

#include <mutex>
#include <memory>
#include <string>
#include <unordered_map>

#include "net.hpp"

namespace seeker {

class NetServiceMgr {
 public:
  using Ptr = std::shared_ptr<NetServiceMgr>;
  using WPtr = std::weak_ptr<NetServiceMgr>;

  static WPtr GetInstance() {
    if (inst_ == nullptr) {
      inst_ = std::make_shared<NetServiceMgr>();
    }
    return inst_;
  }

  bool RegisterService(const std::string& name, INetService::Ptr ptr);
  void UnregisterService(const std::string& name);
  INetService::WPtr GetService(const std::string& name);

 private:
  static Ptr inst_;

  std::mutex mutex_;
  std::unordered_map<std::string, INetService::Ptr> service_;
};

} // namespace seeker

#endif // __SEEKER_SRC_NET_H__