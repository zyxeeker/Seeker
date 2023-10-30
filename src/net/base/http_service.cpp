#include "http_service.h"

#include <iostream>

namespace seeker {
namespace base {

HttpServiceBase::HttpServiceBase(uint16_t port)
    : port_(port) {}

HttpServiceBase::~HttpServiceBase() = default;

bool HttpServiceBase::RegisterRouter(const std::string& name, MsgCallBack cb) {
  std::lock_guard<std::mutex> l(mutex_);
  if (router_.find(name) != router_.end()) {
    return false;
  }
  router_[name] = RouterMeta {
    true,
    std::move(cb)
  };
  return true;
}

void HttpServiceBase::UnregisterRouter(const std::string& name) {
  std::lock_guard<std::mutex> l(mutex_);
  router_.erase(name);
}

bool HttpServiceBase::EnableRouter(const std::string& name) {
  std::lock_guard<std::mutex> l(mutex_);
  if (router_.find(name) == router_.end()) {
    return false;
  }
  router_[name].Enable = true;
  return true;
}

bool HttpServiceBase::DisableRouter(const std::string& name) {
  std::lock_guard<std::mutex> l(mutex_);
  if (router_.find(name) == router_.end()) {
    return false;
  }
  router_[name].Enable = false;
  return true;
}

bool HttpServiceBase::CallRouter(const std::string& name, 
                                const ReqMsgMeta& req, 
                                RespMsgMeta& resp) {
  std::lock_guard<std::mutex> l(mutex_);
  auto res = router_.find(name);
  if (res == router_.end()) {
    return false;
  }
  if (!res->second.Enable) {
    return false;
  }
  auto cb = res->second.CallBack;
  resp = cb(req);
  return true;
}

} // namespace base
} // namespace seeker