/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-25 14:12:10
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-25 18:32:02
 * @Description: 
 */

#ifndef __SEEKER_SRC_IHTTP_SERVICE_H__
#define __SEEKER_SRC_IHTTP_SERVICE_H__

#include <string>
#include <mutex>
#include <memory>
#include <functional>
#include <unordered_map>

#include "net.hpp"

namespace seeker {
namespace base {

class HttpService : public IHttpService {
 public:
  virtual ~HttpService() = default;

  bool RegisterRouter(const std::string& name, HttpMsgCallBack cb) override;
  void UnregisterRouter(const std::string& name) override;
  bool QueryRouter(const std::string& name);
  HttpRespMsgMeta UpdateHttpMsg(const HttpReqMsgMeta& msg);
  
  inline void SetHttpMsgCallBack(HttpMsgCallBack cb) {
    msg_cb_ = cb;
  };

 protected:
  HttpMsgCallBack msg_cb_;

 private:
  std::mutex router_ops_mutex_;
  std::unordered_map<std::string, HttpMsgCallBack> router_;
};

} // namespace base
} // namespace seeker

#endif // __SEEKER_SRC_IHTTP_SERVICE_H__
