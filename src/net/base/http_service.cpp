#include "http_service.h"

#include <iostream>

namespace seeker {
namespace base {

bool HttpService::RegisterRouter(const std::string& name, HttpMsgCallBack cb) {
  std::lock_guard<std::mutex> l(router_ops_mutex_);
  return router_.insert({ name, cb }).second;
}

void HttpService::UnregisterRouter(const std::string& name) {
  std::lock_guard<std::mutex> l(router_ops_mutex_);
  router_.erase(name);
}

HttpRespMsgMeta HttpService::UpdateHttpMsg(const HttpReqMsgMeta& msg) {
  std::lock_guard<std::mutex> l(router_ops_mutex_);
  auto cb = router_[msg.Url];
  if (cb != nullptr) {
    return cb(msg);
  }
  return {};
}

bool HttpService::QueryRouter(const std::string& name) {
  std::lock_guard<std::mutex> l(router_ops_mutex_);
  // std::cout << "URL NAME: " << name << std::endl;
  return router_.find(name) != router_.end();
}

} // namespace base
} // namespace seeker