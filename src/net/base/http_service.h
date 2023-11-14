/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-25 14:12:10
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-14 10:43:04
 * @Description: 
 */

#ifndef __SEEKER_SRC_IHTTP_SERVICE_H__
#define __SEEKER_SRC_IHTTP_SERVICE_H__

#include "../../net.h"

namespace seeker {
namespace base {

class HttpServiceBase : public IHttpService {
  struct RouterMeta {
    RouterBase::Meta::WPtr Router;
  };
 public:
  HttpServiceBase(uint16_t port);
  ~HttpServiceBase();

  virtual bool Start() = 0;
  virtual void Stop() = 0;
  
  bool RegisterRouter(RouterBase::Ptr router) override;
  void UnregisterRouter(RouterBase::Ptr router) override;
  void ListAllRouter() override;

  bool CallRouter(const ReqMeta& req, RespMeta& resp);

  inline uint16_t port() const {
    return port_;
  }

 private:
  bool RegisterRouterImpl(RouterBase::Ptr router);
  
 private:
  std::mutex mutex_;

  bool auth_;
  uint16_t port_;

  std::unordered_map<std::string, std::vector<RouterMeta> > router_;
};

} // namespace base
} // namespace seeker

#endif // __SEEKER_SRC_IHTTP_SERVICE_H__
