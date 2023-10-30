/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-25 09:12:19
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-30 18:02:09
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
    // TODO: Support Code
    MsgMetaComplex Complex;
  };

  using MsgCallBack = std::function<RespMsgMeta(const ReqMsgMeta&)>;
  
  virtual ~IHttpService() = default;

  static WPtr Create(const std::string& name, uint16_t port);

  static void Destory(const std::string& name);

  virtual bool RegisterRouter(const std::string& name, MsgCallBack cb) = 0;
  
  virtual void UnregisterRouter(const std::string& name) = 0;

  virtual bool EnableRouter(const std::string& name) = 0;

  virtual bool DisableRouter(const std::string& name) = 0;
};

} // namespace seeker

#endif // __SEEKER_CFG_HPP__