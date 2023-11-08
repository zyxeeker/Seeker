/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-25 09:12:19
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-11-08 15:30:32
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

  virtual bool Start() = 0;
  
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

  using RouterCallBack = std::function<bool(const ReqMsgMeta&, RespMsgMeta&)>;

  class RouterBase {
   public:
    using Ptr = std::shared_ptr<RouterBase>;
    enum HANDLER_RESULT {
      HANDLER_OK,
      HANDLER_BAD_CONVERT,
      HANDLER_WRONG_METHOD,
    };

    struct Meta {
      using WPtr = std::weak_ptr<Meta>;
      using Ptr = std::shared_ptr<Meta>;

      std::string Name;
      METHOD Method;
      bool Enabled;
      std::function<HANDLER_RESULT(const std::string&, std::string&)> Handler;
    };
    
   public:
    RouterBase(const std::string& preffix) : preffix_(preffix) {}
    virtual ~RouterBase() = default;

    template <class C, typename Ret, typename ...Args>
    bool RegisterGet(const std::string& name, C* c, Ret (C::*m)(Args...));

    template <class C, typename Ret, typename ...Args>
    bool RegisterInvoke(const std::string& name, METHOD method, C* c, Ret (C::*m)(Args...));
    
    const std::string& preffix() const { return preffix_; }
    const std::vector<Meta::Ptr>& meta() const { return meta_; }

   private:
    std::string preffix_;
    std::vector<Meta::Ptr> meta_;
  };

  class AuthRouterBase : public RouterBase {
   public:
    using Ptr = std::shared_ptr<AuthRouterBase>;
    using WPtr = std::weak_ptr<AuthRouterBase>;

    enum AUTH_RESULT {
      OK            = 0,
      UNAUTHORIZED,
      FORBIDDEN
    };

    AuthRouterBase(const std::string& preffix) : RouterBase(preffix) {}
    virtual ~AuthRouterBase() = default;

    virtual AUTH_RESULT OnTokenCallBack(const std::string& router, 
                                        METHOD method, const std::string& token) = 0;
  };

 public:
  virtual ~IHttpService() = default;

  static WPtr Create(const std::string& name, uint16_t port, bool auth = false);

  static void Destory(const std::string& name);

  virtual bool RegisterRouter(RouterBase::Ptr router) = 0;
  
  virtual void UnregisterRouter(RouterBase::Ptr router) = 0;

  virtual bool RegisterAuthRouter(AuthRouterBase::Ptr router) = 0;
  
  virtual void UnregisterAuthRouter() = 0;

  virtual void ListAllRouter() = 0;
};

template <class C, typename Ret, typename ...Args>
bool IHttpService::RouterBase::RegisterGet(const std::string& name, C* c, Ret (C::*m)(Args...)) {
  std::function<Ret(Args...)> func = [=](auto&&... args) { return (c->*m)(std::forward<decltype(args)>(args)...); };

  auto cb = [func](const std::string& req, std::string& resp) {
    auto data = func();
    if (!seeker::ToString<Ret>(data, resp)) {
      return HANDLER_BAD_CONVERT;
    }
    return HANDLER_OK;
  };
  meta_.push_back(Meta::Ptr(new Meta{ name, GET, true, cb }));
  return true;
}

template <class C, typename Ret, typename ...Args>
bool IHttpService::RouterBase::RegisterInvoke(const std::string& name, METHOD method, C* c, Ret (C::*m)(Args...)) {
  std::function<Ret(Args...)> func = [=](auto&&... args) { return (c->*m)(std::forward<decltype(args)>(args)...); };

  using ArgTypes = std::tuple<Args...>;
  using ValueElementType = typename std::tuple_element<0, ArgTypes>::type;
  using ValueType = typename std::remove_reference<ValueElementType>::type;

  auto cb = [func](const std::string& req, std::string& resp) {
    ValueType value;

    if (!seeker::FromString(req, value)) {
      return HANDLER_BAD_CONVERT;
    }
    auto res = func(value);
    if (!seeker::ToString<Ret>(res, resp)) {
      return HANDLER_BAD_CONVERT;
    }
    return HANDLER_OK;
  };
  meta_.push_back(Meta::Ptr(new Meta{ name, method, true, cb }));
  return true;
}

} // namespace seeker

#endif // __SEEKER_NET_HPP__