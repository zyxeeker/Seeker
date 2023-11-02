/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-25 14:12:10
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-02 13:16:16
 * @Description: 
 */

#ifndef __SEEKER_SRC_IHTTP_SERVICE_H__
#define __SEEKER_SRC_IHTTP_SERVICE_H__

#include "../../net.h"

namespace seeker {
namespace base {

class HttpServiceBase : public IHttpService {
  struct RouterMeta {
    bool Enable;
    bool Auth;
    MsgCallBack CallBack;
  };

 public:
  HttpServiceBase(uint16_t port);
  ~HttpServiceBase();

  virtual bool Start() = 0;
  virtual void Stop() = 0;
  bool RegisterRouter(const std::string& name, MsgCallBack cb, bool need_auth) override;
  void UnregisterRouter(const std::string& name) override;
  bool EnableRouter(const std::string& name) override;
  bool DisableRouter(const std::string& name) override;
  bool RegisterAuth(AuthCallBack cb) override;
  void UnregisterAuth() override;
  bool EnableAuth(const std::string& name) override;
  bool DisableAuth(const std::string& name) override;
  
  bool CallRouter(const std::string& name, 
                  const ReqMsgMeta& req, RespMsgMeta& resp);
  inline uint16_t port() const {
    return port_;
  }
 private:
  std::mutex mutex_;
  
  uint16_t port_;

  AuthCallBack auth_cb_;
  std::unordered_map<std::string, RouterMeta> router_;
};

} // namespace base
} // namespace seeker

#endif // __SEEKER_SRC_IHTTP_SERVICE_H__
