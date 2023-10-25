/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2023-10-25 09:12:19
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2023-10-25 18:50:27
 * @Description: 网络服务基本接口
 */

#ifndef __SEEKER_NET_HPP__
#define __SEEKER_NET_HPP__

#include <vector>
#include <string>
#include <functional>

namespace seeker {

enum HTTP_METHOD {
  GET   = 0x0001,
  POST  = 0x0010,
  UNKNOWN
  // TODO: support more
};

struct HttpHeaderMeta {
  std::string Key;
  std::string Value;
};

typedef struct _HttpMsgMeta {
  HTTP_METHOD Method;
  std::string Url;
  std::vector<HttpHeaderMeta> Headers;
  std::string Content;
} HttpReqMsgMeta, HttpRespMsgMeta;

using HttpMsgCallBack = std::function<HttpRespMsgMeta(const HttpReqMsgMeta&)>;

class INetService {
 public:
  enum TYPE {
    SERVICE_HTTP,
    // SERVICE_WS,
  };
  using Ptr = std::shared_ptr<INetService>;
  using WPtr = std::weak_ptr<INetService>;

  virtual ~INetService() = default;

  static WPtr Create(TYPE type, const std::string& name, uint16_t port);

  static void Destory(const std::string& name);

  static WPtr Get(const std::string& name);
  
  virtual bool Start() = 0;

  virtual void Stop() = 0;
};

class IHttpService : public INetService {
 public:
  using Ptr = std::shared_ptr<IHttpService>;

  virtual bool RegisterRouter(const std::string& name, HttpMsgCallBack cb) = 0;
  virtual void UnregisterRouter(const std::string& name) = 0;
};

} // namespace seeker

#endif // __SEEKER_CFG_HPP__