/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-25 09:12:19
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-02 14:17:20
 * @Description: 网络服务基本接口
 */

#ifndef __SEEKER_NET_HPP__
#define __SEEKER_NET_HPP__

#include <vector>
#include <string>
#include <functional>

namespace seeker {

class INetService {
 public:
  using Ptr = std::shared_ptr<INetService>;
  using WPtr = std::weak_ptr<INetService>;

  static void ListAllService();

 public:
  virtual ~INetService() = default;

  virtual bool Start() = 0;
  
  virtual void Stop() = 0; 
};

class IHttpService : public INetService {
 public: 
  using Ptr = std::shared_ptr<IHttpService>;
  using WPtr = std::weak_ptr<IHttpService>;
  
  enum METHOD {
    GET   = 0x0001,
    POST  = 0x0010,
    UNKNOWN
    // TODO: Support More...
  };

  enum AUTH_RESULT {
    OK            = 0,
    UNAUTHORIZED  = 401,
    FORBIDEN      = 403
  };

  struct HeaderMeta {
    std::string Key;
    std::string Value;
  };

  struct MsgMetaComplex {
    std::vector<HeaderMeta> Headers;
    std::string Content;
  };

  struct ReqMsgMeta {
    METHOD Method;
    std::string Url;
    MsgMetaComplex Complex;
  };

  struct RespMsgMeta {
    uint32_t Code;
    MsgMetaComplex Complex;
  };

  using MsgCallBack = std::function<RespMsgMeta(const ReqMsgMeta&)>;
  using AuthCallBack = std::function<AUTH_RESULT(const std::string&)>;
  
  virtual ~IHttpService() = default;

  static WPtr Create(const std::string& name, uint16_t port);

  static void Destory(const std::string& name);

  virtual bool RegisterRouter(const std::string& name, MsgCallBack cb, bool need_auth = false) = 0;
  
  virtual void UnregisterRouter(const std::string& name) = 0;

  virtual bool EnableRouter(const std::string& name) = 0;

  virtual bool DisableRouter(const std::string& name) = 0;

  virtual bool RegisterAuth(AuthCallBack cb) = 0;
  
  virtual void UnregisterAuth() = 0;

  virtual bool EnableAuth(const std::string& name) = 0;

  virtual bool DisableAuth(const std::string& name) = 0;
};

} // namespace seeker

#endif // __SEEKER_CFG_HPP__