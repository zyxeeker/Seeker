/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-25 14:12:10
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-08 15:56:55
 * @Description: 
 */

#ifndef __SEEKER_SRC_IHTTP_SERVICE_H__
#define __SEEKER_SRC_IHTTP_SERVICE_H__

#include "../../net.h"

namespace seeker {
namespace base {

class HttpServiceBase : public IHttpService {
  enum ROUTER_TYPE {
    AUTH_ROUTER,
    NORMAL_ROUTER
  };
  struct RouterMeta {
    ROUTER_TYPE Type;
    RouterBase::Meta::WPtr Router;
  };
 public:
  HttpServiceBase(uint16_t port, bool auth);
  ~HttpServiceBase();

  virtual bool Start() = 0;
  virtual void Stop() = 0;
  bool RegisterRouter(RouterBase::Ptr router) override;
  void UnregisterRouter(RouterBase::Ptr router) override;
  bool RegisterAuthRouter(AuthRouterBase::Ptr router) override;
  void UnregisterAuthRouter() override;
  void ListAllRouter() override;

  bool CallRouter(const ReqMsgMeta& req, RespMsgMeta& resp);

  inline uint16_t port() const {
    return port_;
  }

 private:
  bool RegisterRouterImpl(RouterBase::Ptr router, ROUTER_TYPE type);
  
 private:
  std::mutex mutex_;

  bool auth_;
  uint16_t port_;
  AuthRouterBase::WPtr auth_router_;
  std::unordered_map<std::string, RouterMeta> router_;
};

} // namespace base
} // namespace seeker

#endif // __SEEKER_SRC_IHTTP_SERVICE_H__
