/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-25 09:12:19
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-18 15:46:09
 * @Description: 网络服务基本接口
 */

#ifndef __SEEKER_NET_HPP__
#define __SEEKER_NET_HPP__

#include <vector>
#include <string>
#include <functional>

#include <cfg.hpp>

namespace seeker {

class INetService {
 public:
  using Ptr = std::shared_ptr<INetService>;
  using WPtr = std::weak_ptr<INetService>;

  static void ListAllService();

 public:
  virtual ~INetService() = default;

  virtual bool Start(uint16_t port) = 0;
  
  virtual void Stop() = 0; 
};

class IHttpService : public INetService {
 public: 
  using Ptr = std::shared_ptr<IHttpService>;
  using WPtr = std::weak_ptr<IHttpService>;

  enum METHOD {
    UNKNOWN = 0,
    GET     = 0x0001,
    POST    = 0x0010,
    // TODO: Support More...
  };

  struct HeaderMeta {
    std::string Key;
    std::string Value;
  };

  typedef struct MsgMetaComplex {
    std::vector<HeaderMeta> Headers;
    std::string Content;
  } ReqMsgMeta, RespMsgMeta;

  struct ReqMeta {
    METHOD Method;
    std::string Url;
    std::string Query;
    MsgMetaComplex Complex;
  };

  struct RespMeta {
    uint32_t Code;
    MsgMetaComplex Complex;
  };

  class RouterBase {
   public:
    using Ptr = std::shared_ptr<RouterBase>;
    enum HANDLER_RESULT {
      HANDLER_OK,
      HANDLER_BAD_CONVERT,
      HANDLER_NOT_FOUND,
      HANDLER_UNAUTHORIZED,
      HANDLER_FORBIDDEN
    };

    using HandlerCallback = std::function<HANDLER_RESULT(const ReqMsgMeta&, RespMsgMeta&)>;

    struct Meta {
      using WPtr = std::weak_ptr<Meta>;
      using Ptr = std::shared_ptr<Meta>;

      std::string Name;
      METHOD Method;
      bool Enabled;
      HandlerCallback Handler;
    };

   public:
    RouterBase(const std::string& preffix) : preffix_(preffix) {}
    virtual ~RouterBase() = default;

    bool Register(const std::string& name, METHOD method, HandlerCallback handler) {
      meta_.emplace_back(Meta::Ptr(new Meta{ name, method, true, handler }));
      return true;
    }

    void UnRegister(const std::string& name) {
      for (auto i = meta_.begin(); i != meta_.end(); i++) {
        if ((*i)->Name == name) {
          meta_.erase(i);
          return;
        }
      }
    }

    const std::string& preffix() const { return preffix_; }
    const std::vector<Meta::Ptr>& meta() const { return meta_; }

   private:
    std::string preffix_;
    std::vector<Meta::Ptr> meta_;
  };

 public:
  virtual ~IHttpService() = default;

  static Ptr Create(const std::string& name);

  virtual bool RegisterRouter(RouterBase::Ptr router) = 0;
  
  virtual void UnregisterRouter(RouterBase::Ptr router) = 0;

  virtual void ListAllRouter() = 0;

  virtual void SetWebSite(const std::string& path) = 0;
};

} // namespace seeker

#endif // __SEEKER_NET_HPP__