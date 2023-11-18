/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-25 14:12:10
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-18 11:31:36
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
  enum CALL_RESULT {
    NOT_FOUND,
    UNSUPPORT,
    OK,
  };
  HttpServiceBase();
  ~HttpServiceBase();

  virtual bool Start(uint16_t port) = 0;
  virtual void Stop() = 0;
  
  bool RegisterRouter(RouterBase::Ptr router) override;
  void UnregisterRouter(RouterBase::Ptr router) override;
  void ListAllRouter() override;

  virtual void SetWebSite(const std::string& path) = 0;

  bool CallRouter(const ReqMeta& req, RespMeta& resp);

 private:
  bool RegisterRouterImpl(RouterBase::Ptr router);
  
 private:
  std::mutex mutex_;

  std::unordered_map<std::string, std::vector<RouterMeta> > router_;
};

} // namespace base
} // namespace seeker

#endif // __SEEKER_SRC_IHTTP_SERVICE_H__
