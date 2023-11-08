#include "http_service.h"

#include <iostream>

namespace seeker {
namespace base {

HttpServiceBase::HttpServiceBase(uint16_t port, bool auth)
    : auth_(auth), 
      port_(port) {}

HttpServiceBase::~HttpServiceBase() = default;

bool HttpServiceBase::RegisterRouter(RouterBase::Ptr router) {
  std::lock_guard<std::mutex> l(mutex_);
  return RegisterRouterImpl(router, NORMAL_ROUTER);
}

void HttpServiceBase::UnregisterRouter(RouterBase::Ptr router) {
  std::lock_guard<std::mutex> l(mutex_);
  for (auto& i : router->meta()) {
    router_.erase(router->preffix() + i->Name);
  }
}

bool HttpServiceBase::RegisterAuthRouter(AuthRouterBase::Ptr router) {
  std::lock_guard<std::mutex> l(mutex_);
  if (!auth_router_.expired()) {
    std::cout << "already had a auth!" << std::endl;
    return false;
  }
  auth_router_ = router;
  return RegisterRouterImpl(std::dynamic_pointer_cast<RouterBase>(router), AUTH_ROUTER);
}

void HttpServiceBase::UnregisterAuthRouter() {
  std::lock_guard<std::mutex> l(mutex_);
  auth_router_.reset();
}

void HttpServiceBase::ListAllRouter() {
  for (auto& i : router_) {
    std::cout << i.first << std::endl;
  }
}

bool HttpServiceBase::CallRouter(const ReqMsgMeta& req, RespMsgMeta& resp) {
  std::lock_guard<std::mutex> l(mutex_);
  auto res = router_.find(req.Url);
  if (res == router_.end() || (res != router_.end() && res->second.Router.expired())) {
    resp.Code = 404;
    return true;
  }
  
  if (auth_) {
    if (auth_router_.expired()) {
      resp.Code = 401;
      return true;
    }

    std::string token;
    for (auto& i : req.Complex.Headers) {
      if (i.Key == "Authorization") {
        token = i.Value;
        break;
      }
    }

    if (res->second.Type == NORMAL_ROUTER) {
      switch (auth_router_.lock()->OnTokenCallBack(req.Url, req.Method, token)) {
        case AuthRouterBase::OK:
          break;
        case AuthRouterBase::FORBIDDEN:
          resp.Code = 403;
          return true;
        case AuthRouterBase::UNAUTHORIZED:
        default:
          resp.Code = 401;
          return true;
      }
    }
  }

  auto ptr = res->second.Router.lock();  
  if (ptr->Enabled && (req.Method & ptr->Method)) {
    auto code = ptr->Handler(req.Complex.Content, resp.Complex.Content);
    switch (code) {
      case RouterBase::HANDLER_OK:
        resp.Code = 200;
        break;
      case RouterBase::HANDLER_BAD_CONVERT:
        resp.Code = 400;
        break;
      default:
        resp.Code = 503;
        break;
    }
  } else {
    resp.Code = 405;
  }

  return true;
}

bool HttpServiceBase::RegisterRouterImpl(RouterBase::Ptr router, ROUTER_TYPE type) {
    for (auto& i : router->meta()) {
    auto res = router_.insert({ 
      router->preffix() + (i->Name.empty() ? "" : "/" + i->Name), 
        { type, i }
      });
    if (!res.second) {
      std::cout << "conflict router: " << router->preffix() + i->Name << std::endl;
      return false;
    }
  }
  return true;
}

} // namespace base
} // namespace seeker