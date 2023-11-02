#include "http_service.h"

#include <iostream>

namespace seeker {
namespace base {

HttpServiceBase::HttpServiceBase(uint16_t port)
    : port_(port) {}

HttpServiceBase::~HttpServiceBase() = default;

bool HttpServiceBase::RegisterRouter(const std::string& name, MsgCallBack cb, bool need_auth) {
  std::lock_guard<std::mutex> l(mutex_);
  if (router_.find(name) != router_.end()) {
    return false;
  }
  router_[name] = RouterMeta {
    true,
    need_auth,
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

bool HttpServiceBase::RegisterAuth(AuthCallBack cb) {
  std::lock_guard<std::mutex> l(mutex_);
  if (auth_cb_) {
    return false;
  }
  auth_cb_ = cb;
  return true;
}

void HttpServiceBase::UnregisterAuth() {
  std::lock_guard<std::mutex> l(mutex_);
  auth_cb_ = nullptr;
}

bool HttpServiceBase::EnableAuth(const std::string& name) {
  std::lock_guard<std::mutex> l(mutex_);
  if (router_.find(name) == router_.end()) {
    return false;
  }
  router_[name].Auth = true;
  return true;
}

bool HttpServiceBase::DisableAuth(const std::string& name) {
  std::lock_guard<std::mutex> l(mutex_);
  if (router_.find(name) == router_.end()) {
    return false;
  }
  router_[name].Auth = false;
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

  if (res->second.Auth && auth_cb_) {
    AUTH_RESULT auth_res = UNAUTHORIZED;
    for (auto& i : req.Complex.Headers) {
      if (i.Key == "Authorization") {
        auth_res = (auth_cb_)(i.Value);
        break;
      }
    }
    if (auth_res != OK) {
      resp.Code = auth_res;
      return true;
    }
  }

  auto cb = res->second.CallBack;
  resp = cb(req);
  return true;
}

} // namespace base
} // namespace seeker