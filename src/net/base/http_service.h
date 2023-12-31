/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-25 14:12:10
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-22 09:35:21
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
  enum QUERY_ROUTER_RESULT {
    QUERY_OK,
    QUERY_NOT_ALLOWED,
    QUERY_NOT_FOUND,
  };

  HttpServiceBase();
  ~HttpServiceBase();

  virtual bool Start(uint16_t port) = 0;
  virtual void Stop() = 0;
  
  bool RegisterRouter(RouterBase::Ptr router) override;
  void UnregisterRouter(RouterBase::Ptr router) override;
  void ListAllRouter() override;

  virtual void SetWebSite(const std::string &path) = 0;

  QUERY_ROUTER_RESULT QueryRouter(const std::string &url, METHOD method, RouterBase::Meta::Ptr &ptr);
  bool CallRouter(const RouterBase::Meta::Ptr &ptr, const ReqMeta &req, RespMeta &resp);
  bool CallFileRouter(const RouterBase::Meta::Ptr &ptr, 
                      const ReqMeta &req, RespMeta &resp,
                      const std::string name, const char* buff, size_t len);
 private:
  bool RegisterRouterImpl(RouterBase::Ptr router);
  bool CheckRouterResult(RouterBase::HANDLER_RESULT code, RespMeta &resp);
  
 private:
  std::mutex mutex_;

  std::unordered_map<std::string, std::vector<RouterBase::Meta::WPtr> > router_;
};

} // namespace base
} // namespace seeker

#endif // __SEEKER_SRC_IHTTP_SERVICE_H__
