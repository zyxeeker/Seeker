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
    log::Info() << i.first << std::endl;
  }
}

bool HttpServiceBase::CallRouter(const ReqMeta& req, RespMeta& resp) {
  std::lock_guard<std::mutex> l(mutex_);

  auto res = router_.find(req.Url);
  if (res == router_.end()) {
    return false;
  }

  RouterBase::Meta::Ptr ptr;
  for (auto &i : res->second) {
    if (!i.Router.expired()) {
      ptr = i.Router.lock();
      if (ptr->Enabled) {
        if (req.Method & ptr->Method) {
          break;
        } else {
          resp.Code = 405;
          ptr.reset();
        }
      } else {
        resp.Code = 404;
        ptr.reset();
      }
    }
  }

  if (!ptr) {
    return true;
  }
  
  auto code = ptr->Handler(req.Complex, resp.Complex);
  switch (code) {
    case RouterBase::HANDLER_OK:
      resp.Code = 200;
      break;
    case RouterBase::HANDLER_BAD_CONVERT:
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
    default:
      resp.Code = 503;
      break;
  }

  return true;
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
        if (!j.Router.expired() && (j.Router.lock()->Method & i->Method)) {
          conflict = true;
        }
      }
      if (conflict) {
        seeker::log::Warn() << "conflict router path: " << api << std::endl;
      } else {
        res->second.push_back({ i });
      }
    }
  }
  return true;
}

} // namespace base
} // namespace seeker