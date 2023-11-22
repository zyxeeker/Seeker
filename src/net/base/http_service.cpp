#include "http_service.h"

#include "log.h"

namespace seeker {
namespace base {

HttpServiceBase::HttpServiceBase() = default;

HttpServiceBase::~HttpServiceBase() = default;

bool HttpServiceBase::RegisterRouter(RouterBase::Ptr router) {
  std::lock_guard<std::mutex> l(mutex_);
  return RegisterRouterImpl(router);
}

void HttpServiceBase::UnregisterRouter(RouterBase::Ptr router) {
  std::lock_guard<std::mutex> l(mutex_);
  for (auto& i : router->meta()) {
    router_.erase(router->preffix() + i->Name);
  }
}

void HttpServiceBase::ListAllRouter() {
  for (auto& i : router_) {
    log::Info() << i.first;
  }
}

bool HttpServiceBase::QueryRouter(const std::string &url, RouterBase::Meta::Ptr &ptr) {
  std::lock_guard<std::mutex> l(mutex_);

  auto res = router_.find(url);
  if (res == router_.end()) {
    return false;
  }
  for (auto &i : res->second) {
    if (!i.expired()) {
      ptr = i.lock();
      return true;
    }
  }
  return false;
}

bool HttpServiceBase::CallRouter(const RouterBase::Meta::Ptr &ptr, const ReqMeta &req, RespMeta &resp) {
  std::lock_guard<std::mutex> l(mutex_);
  if (ptr->Enabled) {
    if (!(req.Method & ptr->Method)) {
      resp.Code = 405;
      return false;
    }
  } else {
    resp.Code = 404;
    return false;
  }

  auto code = ptr->ReqHandler(req.Complex, resp.Complex);
  return CheckRouterResult(code, resp);
}

bool HttpServiceBase::CallFileRouter(const RouterBase::Meta::Ptr &ptr, 
                                     const ReqMeta &req, RespMeta &resp,
                                     const std::string name, const char* buff, size_t len) {
  if (ptr->Enabled) {
    if (!(req.Method & ptr->Method)) {
      resp.Code = 405;
      return false;
    }
  } else {
    resp.Code = 404;
    return false;
  }
  auto code = ptr->FileReqHandler(req.Complex, name, buff, len, resp.Complex);
  return CheckRouterResult(code, resp);
}

bool HttpServiceBase::RegisterRouterImpl(RouterBase::Ptr router) {
  for (auto &i : router->meta()) {
    auto api = router->preffix() + (i->Name.empty() ? "" : "/" + i->Name);
    auto res = router_.find(api);
    if (res == router_.end()) {
      router_.insert({ 
        api, 
        {{ i }}
      });
    } else {
      bool conflict = false;
      for (auto &j : res->second) {
        if (!j.expired() && (j.lock()->Method & i->Method)) {
          conflict = true;
        }
      }
      if (conflict) {
        seeker::log::Warn() << "conflict router path: " << api;
      } else {
        res->second.push_back({ i });
      }
    }
  }
  return true;
}

bool HttpServiceBase::CheckRouterResult(RouterBase::HANDLER_RESULT code, RespMeta &resp) {
    switch (code) {
    case RouterBase::HANDLER_OK:
      resp.Code = 200;
      return true;
    case RouterBase::HANDLER_BAD:
      resp.Code = 400;
      break;
    case RouterBase::HANDLER_NOT_FOUND:
      resp.Code = 404;
      break;
    case RouterBase::HANDLER_UNAUTHORIZED:
      resp.Code = 401;
      break;
    case RouterBase::HANDLER_FORBIDDEN:
      resp.Code = 403;
      break;
    case RouterBase::HANDLER_REQ_TOO_LARGE:
      resp.Code = 413;
      break;
    default:
      resp.Code = 503;
      break;
  }
  return false;
}

} // namespace base
} // namespace seeker